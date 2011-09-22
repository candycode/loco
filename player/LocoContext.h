#pragma once
//#SRCHEADER

#include <cstdlib> //for dupenv (win), putenv, getenv

#ifdef LOCO_WKIT
#include <QtWebKit/QWebFrame>
#endif

#include <QCoreApplication>
#ifdef LOCO_GUI
#include <QApplication>
#endif

#include <QString>
#include <QList>
#include <QVariant>
#include <QVariantMap>
#include <QPluginLoader>
#include <QProcess>
#include <QProcessEnvironment>
#include <QDir>
#include <QSharedPointer>
#include <QtNetwork/QNetworkAccessManager>
#include <QSet>
#include <QUrl>
#include <QRegExp>

#include <algorithm>
#include <cstdlib>

#include "EWL.h"
#include "LocoFilter.h"
#include "LocoObject.h"
#include "LocoConsole.h"
#include "LocoScriptFilter.h"
#include "LocoFileSystem.h"
#include "LocoSystem.h"
#include "LocoNetworkAccessManager.h"
#include "LocoFileAccessManager.h"
#include "LocoObjectInfo.h"
#include "LocoIJSInterpreter.h"

#ifdef LOCO_GUI
typedef QApplication LocoQtApp;
#else
typedef QCoreApplication LocoQtApp;
#endif


namespace loco {

typedef QMap< QString, Filter*  > Filters;
typedef QList< QPluginLoader* > PluginLoaders;
typedef QList< Object* > JScriptObjCtxInstances; 

typedef QMap< bool, QMap< QString, Object* > > URIObjectMap;
typedef QMap< QString, Filter* > URIFilterMap;

typedef QList< QPair< QRegExp, QStringList > > NameFilterMap;


struct IJavaScriptInit {
    virtual void SetContext( Context*  ) = 0;
    virtual QString GenerateCode() const = 0;
    virtual ~IJavaScriptInit() {}
};



class JSContext;

class Context : public EWL {
    Q_OBJECT  

friend class App; //loco contexts are embedded into an application

public:
    
    Context( Context* parent = 0 );
   
    Context( IJSInterpreter* wf, LocoQtApp* app, const QStringList& cmdLine,
             Context* parent = 0 );

    void Init( IJSInterpreter* wf, LocoQtApp* app = 0, 
               const QStringList& cmdLine = QStringList(), Context* parent = 0 );
// called from C++
public:

    QString GetJSInitCode() const { return jsInitGenerator_->GenerateCode(); }

    void AddNameFilterMapping( const QRegExp& rx, const QStringList& filterIds ) {
        nameFilterMap_.push_back( qMakePair( rx, filterIds ) );
    }

    void RemoveNameFilterMapping( const QRegExp& rx ) {
        for( NameFilterMap::iterator i = nameFilterMap_.begin();
             i != nameFilterMap_.end(); ++i ) {
            if( i->first == rx ) {
                nameFilterMap_.erase( i );
                break;
            }
        } 
    }

    void ResetNameFilterMap() { nameFilterMap_.clear(); }

    void SetAppInfo( QSharedPointer< ObjectInfo > ai ) { appInfo_ = ai; }

    // object ownership is transferred to context 
    
    void AddContextToJS(); 

    void AddJSStdObject( Object* obj, bool immediateAdd = false ) { 
        jscriptStdObjects_.push_back( obj );
        if( obj->GetContext() == 0 ) obj->SetContext( this );
        if( obj->GetPluginLoader() == 0 && obj->parent() == 0 ) obj->setParent( this );
        ConnectErrCBack( obj );
        if( immediateAdd ) {
            jsInterpreter_->AddObjectToJS( obj->jsInstanceName(), obj );
        }
    }
    
    void AddJSCtxObject( Object* obj, bool immediateAdd = false ) {
        jscriptCtxInstances_.push_back( obj );
        if( obj->GetContext() == 0 ) obj->SetContext( this );
        if( obj->GetPluginLoader() == 0 && obj->parent() == 0 ) obj->setParent( this );
        ConnectErrCBack( obj );
        if( immediateAdd ) {
            jsInterpreter_->AddObjectToJS( obj->jsInstanceName(), obj );
        }
       
    }

    void RemoveJSCtxObject( Object* obj ) {
        JScriptObjCtxInstances::iterator i = 
            std::find( jscriptCtxInstances_.begin(), jscriptCtxInstances_.end(), obj );
        if( i != jscriptCtxInstances_.end() ) jscriptCtxInstances_.erase( i );
    }

    // call this method from factory objects willing to add new objects into
    // the current context and have the object's lifetime managed by javascript
    // NEVER add root objects from plugins because such objects must be deleted
    // through the plusgin loader's unload method
    QVariant AddObjToJSContext( Object* obj ) {
        jsInterpreter_->AddObjectToJS( obj->jsInstanceName(),
                                       obj,
                                       QScriptEngine::ScriptOwnership );
        obj->SetContext( this );
        ConnectErrCBack( obj );
        connect( obj, SIGNAL( destroyed( QObject* ) ), this, SLOT( RemoveScopeObject( QObject* ) ) );
        instanceObjs_.push_back( obj );
        return jsInterpreter_->EvaluateJavaScript( obj->jsInstanceName() );
    }

  
    void AddFilter( const QString& id, Filter* f ) { 
        if( f->GetPluginLoader() == 0 && f->parent() == 0 ) f->setParent( this );
        filters_[ id ] = f;
    }

    Object* Find( const QString& jsInstanceName ) const;

    void SetJSGlobalNameForContext( const QString& n ) {
        globalContextJSName_ = n;
    }

    const QString& GetJSGlobalNameForContext() const {
        return globalContextJSName_;
    }

	void SetJSInitializer( QSharedPointer< IJavaScriptInit > jsi ) { 
        if( jsi == jsInitGenerator_ ) return;
        jsInitGenerator_ = jsi;
    } 

	QSharedPointer< IJavaScriptInit > GetJSInitializer() const { return jsInitGenerator_; }
      
    void SetJSErrCBack( const QString& code,
                        const QStringList& filterIds = QStringList() ) {
        jsErrCBack_ = Filter( code, filterIds );
    }

    const QString& GetJSErrCBack() const { return jsErrCBack_; }

    void SetJSInitGenerator( QSharedPointer< IJavaScriptInit > jsi ) {
        if( jsi == jsInitGenerator_ ) return;
        jsInitGenerator_ = jsi;
    }

    QSharedPointer< IJavaScriptInit > GetJSInitGenerator() const { return jsInitGenerator_; }

	const JScriptObjCtxInstances& GetStdJSObjects() const { return jscriptStdObjects_; }

    void SetNetworkAccessManager( NetworkAccessManager* nam ) { 
        if( nam == netAccessMgr_ && nam != 0 ) return;
        if( netAccessMgr_ != 0 ) {
            disconnect( netAccessMgr_, SIGNAL( UrlAccessDenied( QString ) ),
                        this, SLOT( OnNetworkRequestDenied( QString ) ) );
            disconnect( netAccessMgr_, SIGNAL( UnauthorizedNetworkAccessAttempt() ),
                        this, SLOT( OnUnauthorizedNetworkAccess() ) );
        }
        netAccessMgr_ = nam;
        if( netAccessMgr_ ) {
            connect( netAccessMgr_, SIGNAL( UrlAccessDenied( QString ) ),
                     this, SLOT( OnNetworkRequestDenied( QString ) ) );
            connect( netAccessMgr_, SIGNAL( UnauthorizedNetworkAccessAttempt() ),
                     this, SLOT( OnUnauthorizedNetworkAccess() ) ); 
        }
    }

    QNetworkAccessManager* GetNetworkAccessManager() const { return netAccessMgr_; }

    void SetFileAccessManager( FileAccessManager* fm ) { fileAccessMgr_ = fm; }

    const FileAccessManager* GetFileAccessManager() const { return fileAccessMgr_; }

    void SetMaxRedirections( int r ) { maxNetRedirections_ = r; }

    int GetMaxRedirection() const { return maxNetRedirections_; }

    void SetNetReadTimeout( int ms ) { readNetworkTimeout_ = ms; }

    int GetNetReadTimeout() const { return readNetworkTimeout_; }

	void Exit( int r ) { app_->exit( r ); ::exit( r ); }

 // attched to internal signals            
private slots:
   

    void RemoveScopeObject( QObject* o ) {
        Object* obj = qobject_cast< Object* >( o );
        JScriptObjCtxInstances::iterator i = 
            std::find( instanceObjs_.begin(), instanceObjs_.end(), obj );
        if( i != instanceObjs_.end() ) instanceObjs_.erase( i ); 
    }

    void OnUnauthorizedNetworkAccess() {
        error( "Unauthorized network access attempted" );
    }
    
    void OnNetworkRequestDenied( QString url ) {
        error( "Unauthorized access to " + url + " attempted" ); 
    }
    
    /// 
    void InitJScript() {
        if( jsInitCode_.isEmpty() ) jsInitCode_ = jsInitGenerator_->GenerateCode();
        jsInterpreter_->EvaluateJavaScript( jsInitCode_ );
    }

    /// this slot can be called from child contexts to make objects
    /// in the parent context available within the child context
    
    void AddJSCtxObjects( IJSInterpreter* wf ) {
        for( JScriptObjCtxInstances::const_iterator i = jscriptCtxInstances_.begin();
             i != jscriptCtxInstances_.end(); ++i ) {
            if( (*i)->GetContext() == 0 ) (*i)->SetContext( this );
            ConnectErrCBack( *i );
            if( (*i)->GetPluginLoader() == 0 && (*i)->parent() == 0 ) (*i)->setParent( this );
            wf->AddObjectToJS( (*i)->jsInstanceName(), *i );  
        }
    }

    /// Add javascript objects that need to be available at initialization time
    void AddJavaScriptObjects() {
        AddJSStdObjects( jsInterpreter_ );
        ///@todo sjould we add parent's context std objects or parent's instance objects ?
        /// Parent()->jscriptsStdObjects()
    }

    /// Remove instance objects created during context operations
    void RemoveInstanceObjects() {
        for( JScriptObjCtxInstances::iterator i = jscriptCtxInstances_.begin();
            i != jscriptCtxInstances_.end(); ++i ) {
            if( (*i)->GetContext() == this ) (*i)->destroy();
        }
        for( PluginLoaders::iterator i = ctxPluginLoaders_.begin();
            i != ctxPluginLoaders_.end(); ++i ) {
            (*i)->unload(); // <- this call deletes the root object;
                            //    the objects generated by the root object are
                            //    garbage collected by the javascript environment
            (*i)->deleteLater();
        }
        jscriptCtxInstances_.clear();
        ctxPluginLoaders_.clear();
        const bool TEMPORARY = false;
        uriObjectMap_[ TEMPORARY ].clear();
        instanceObjs_.clear();
    }

    void RemoveStdObjects() {
        for( JScriptObjCtxInstances::iterator i = jscriptStdObjects_.begin();
            i != jscriptStdObjects_.end(); ++i ) {
            if( (*i)->GetContext() == this && (*i)->GetPluginLoader() == 0 && 
                ( (*i)->parent() == this || (*i)->parent() == 0 ) ) (*i)->destroy();
        }      
        for( PluginLoaders::iterator i = stdPluginLoaders_.begin();
            i != stdPluginLoaders_.end(); ++i ) {
            (*i)->unload(); // <- delete root object
            (*i)->deleteLater();
        }
        jscriptStdObjects_.clear();
        stdPluginLoaders_.clear();
        const bool PERSISTENT = true;
        uriObjectMap_[ PERSISTENT ].clear();
    }    

    void RemoveFilters() {
        for( PluginLoaders::iterator i = filterPluginLoaders_.begin();
             i != filterPluginLoaders_.end(); ++i ) {
            (*i)->unload(); // deletes all root (created with instance()) objects
            (*i)->deleteLater();
        } 
        filters_.clear();
        filterPluginLoaders_.clear();
        ResetNameFilterMap(); 
    }
	

public slots:
    // loco::Objects should be connected to this slot to have errors handled by the context
    // and indirectly by javascript
    // an object RaiseError signal must be connected to one and only one context instance
    void OnObjectError( const QString& err ) {
        error( err );
    }
    void OnFilterError( const QString& err ) {
        error( err );
    }
    // connect to this from e.g. loco::App
    void OnExternalError( const QString& err ) {
        error( err );
    }
    // allow to add objects to other contexts
    void AddJSStdObjects( IJSInterpreter* wf ) {
        for( JScriptObjCtxInstances::const_iterator i = jscriptStdObjects_.begin();
             i != jscriptStdObjects_.end(); ++i ) {
            if( (*i)->GetContext() == 0 ) (*i)->SetContext( this );      
            ConnectErrCBack( *i );
            if( (*i)->GetPluginLoader() == 0 && (*i)->parent() == 0 ) (*i)->setParent( this );
            wf->AddObjectToJS( (*i)->jsInstanceName(), *i );  
        } 
    }

#ifdef LOCO_WKIT
	// allow to add objects to other frames
    void AddJSStdObjects( QWebFrame* wf ) {
        for( JScriptObjCtxInstances::const_iterator i = jscriptStdObjects_.begin();
             i != jscriptStdObjects_.end(); ++i ) {
            if( (*i)->GetContext() == 0 ) (*i)->SetContext( this );      
            ConnectErrCBack( *i );
            if( (*i)->GetPluginLoader() == 0 && (*i)->parent() == 0 ) (*i)->setParent( this );
            wf->addToJavaScriptWindowObject( (*i)->jsInstanceName(), *i );  
        } 
    }
#endif

private slots:
    void OnSelfError( const QString& err ) {
        jsInterpreter_->EvaluateJavaScript( jsErrCBack_ );
    }

private:
    
    void ConnectErrCBack( Object* obj );

private: //js interface invoked from JSContext

friend class JSContext;

public: 
    // Eval needs to be accessible for starting script execution
    QVariant Eval( QString code, const QStringList& filters = QStringList() ) { 
       code = Filter( code, filters );
       if( !error() ) return jsInterpreter_->EvaluateJavaScript( code );
       else {
           jsInterpreter_->EvaluateJavaScript( jsErrCBack_ );
           return QVariant();
       } 
    }


    ObjectInfo* GetAppInfo() const { return appInfo_.data(); }

    // DataT = QByteArray OR QString
    template < typename DataT >
    DataT Filter( DataT data, const QStringList& filterIds = QStringList() ) {
        for( QStringList::const_iterator f = filterIds.begin();
             f != filterIds.end(); ++f ) { 
            Filters::iterator i = filters_.find( *f );
            if( i != filters_.end() ) {
                ::loco::Filter* fp =  i.value();
                data = fp->Apply( data );
                if( fp->error() ) break;                 
            }
            else {
    	        error( "filter id " + *f + " not found" );
    	        break;
            }
        }
        return error() ? DataT() : data; 
    }

private:

    QVariant LoadObject( const QString& uri,  //used as a regular file/resource path for now
                         bool persistent = false );
#ifdef LOCO_GUI
    int Exec() { return app_->exec(); }
#endif
    void LoadFilter( const QString& id, const QString& uri ) {
        if( !fileAccessMgr_->CheckAccess( uri ) ) {
            error( "Access to " + uri + " not allowed" );
            return;
        }
        if( uriFilterMap_.find( uri ) != uriFilterMap_.end() ) return;
        QPluginLoader* pl = new QPluginLoader( uri );
        if( !pl->load() ) {
            error( pl->errorString() );
            jsInterpreter_->EvaluateJavaScript( jsErrCBack_ );
            delete pl;
        } else {
            loco::Filter* lf = qobject_cast< loco::Filter* >( pl->instance() );
            if( lf == 0 ) {
                error( "Wrong filter type" );
                jsInterpreter_->EvaluateJavaScript( jsErrCBack_ );
                delete pl;    
            }
            filters_[ id ] = lf;
            connect( lf, SIGNAL( onError( const QString& ) ), 
                     this, SLOT( OnFilterError( const QString& ) ) );
            filterPluginLoaders_.push_back( pl );
            uriFilterMap_[ uri ] = lf;
        }      
    }
    
    bool HasFilter( const QString& id ) { return filters_.find( id ) != filters_.end(); }

    void AddScriptFilter( const QString& id,
                          const QString& jfun,
						  const QString& jcode = "",
                          const QString& jerrfun = "",
                          const QString& codePlaceHolder = "" ) {
        ::loco::Filter* lf = new ScriptFilter( jsInterpreter_, jfun, jcode, jerrfun, codePlaceHolder );
        connect( lf, SIGNAL( onError( const QString& ) ), 
                                      this, SLOT( OnFilterError( const QString& ) ) );
        filters_[ id ] = lf;
    }

    void LoadScriptFilter( const QString& id,
		                   const QString& uri,
						   const QString& jfun,
                           const QString& jerrfun = "",
                           const QString& codePlaceHolder = "" ) {
        if( !fileAccessMgr_->CheckAccess( uri ) ) {
            error( "Access to " + uri + " not allowed" );
            return;
        }
        QString f = Read( uri );
        if( f.isEmpty() ) return;   
        ::loco::Filter* lf = new ScriptFilter( jsInterpreter_, jfun, f, jerrfun, codePlaceHolder );
        connect( lf, SIGNAL( onError( const QString& ) ), 
                                      this, SLOT( OnFilterError( const QString& ) ) );
        filters_[ id ] = lf;
    }


    QStringList Cmdline() const { return cmdLine_; }

    void RegisterJSErrCBack( const QString& code, const QStringList& filters = QStringList() ) { 
        jsErrCBack_ = Filter( code, filters );
    }

    QByteArray Read( const QString& uri, const QStringList& filters = QStringList() ) {
        // resources: ':/' -> file; 'qrc://' -> url
        QByteArray ret = uri.contains( "://" ) ? ReadUrl( uri ) : ReadFile( uri );
        if( !ret.isEmpty() ) return Filter( ret, filters );
        return ret;
    }

    QVariant Insert( const QString& uri, const QStringList& filters = QStringList() ) {
        QString code;
        if( filters.isEmpty() && autoMapFilters_ ) {
            for( NameFilterMap::iterator i = nameFilterMap_.begin();
                 i != nameFilterMap_.end(); ++i ) {
                if( i->first.exactMatch( uri ) ) {
                    Read( uri, i->second );
                    break;
                }
            }    
        }
        else code = Read( uri, filters );
        if( code.isEmpty() ) return QVariant();
        return Eval( code );
    }

    QVariant Include( QString uri, const QStringList& filters = QStringList() ) {
        if( !uri.startsWith( ":" ) && !uri.contains( "://" ) ) {
            QDir d;
            uri = d.absoluteFilePath( uri );    
        } 
        if( includeSet_.find( uri ) != includeSet_.end() ) return QVariant(); 
        return Insert( uri, filters );
    } 

private:
    QByteArray ReadUrl( const QString& url, QSet< QUrl > redirects = QSet< QUrl >() );

    QByteArray ReadFile( const QString& f );

    void jsErr() { jsInterpreter_->EvaluateJavaScript( jsErrCBack_ ); }

private:
    QSharedPointer< JSContext > jsContext_;
    IJSInterpreter* jsInterpreter_;
    LocoQtApp* app_;
    Context* parent_;
    QStringList cmdLine_;
   
private:
    Filters filters_;
    PluginLoaders stdPluginLoaders_;
    PluginLoaders ctxPluginLoaders_;
    PluginLoaders filterPluginLoaders_;    
    JScriptObjCtxInstances jscriptStdObjects_;
    JScriptObjCtxInstances jscriptCtxInstances_;
    JScriptObjCtxInstances instanceObjs_; 
    QString jsErrCBack_;
    URIObjectMap uriObjectMap_;
    URIFilterMap uriFilterMap_;
    QString globalContextJSName_;
    QString jsInitCode_;
    NameFilterMap nameFilterMap_;
    bool autoMapFilters_;
private: 
    QSharedPointer< IJavaScriptInit > jsInitGenerator_; 
    NetworkAccessManager* netAccessMgr_;
    FileAccessManager* fileAccessMgr_;
private:
    QSet< QString > includeSet_;
private:
    int readNetworkTimeout_;
    int maxNetRedirections_;
    QSharedPointer< ObjectInfo > appInfo_;        
};

//==============================================================================

class JSContext : public Object {
    Q_OBJECT
public:
    JSContext( Context& ctx ) : Object( 0, "LocoContext", "Loco/Context" ),
    ctx_( ctx )  {
        SetDestroyable( false );       
    }

// invocable from javascript
public slots: // js interface
    
	void exit( int code ) { ctx_.Exit( code ); }

	QVariant include( const QString& path, const QStringList& filters = QStringList() ) {
        return ctx_.Include( path, filters );
    }

    QVariant insert( const QString& path, const QStringList& filters = QStringList() ) {
        return ctx_.Insert( path, filters );
    }
    
	QString read( const QString& path, const QStringList& filters = QStringList() ) {
	    return ctx_.Read( path, filters );
    }
    
	QStringList pluginPath() const { return QCoreApplication::libraryPaths(); }

	void setPluginPath( const QStringList& paths ) { QCoreApplication::setLibraryPaths( paths ); }

	QString appFilePath() const { return QCoreApplication::applicationFilePath(); }

	QString appDirPath() const { return QCoreApplication::applicationDirPath(); }

	QString appName() const { return QCoreApplication::applicationName(); }

	QString appVersion() const { return QCoreApplication::applicationVersion(); }

	QString appVendor() const { return QCoreApplication::organizationName(); }

	QString currentDir() const { return QDir::current().absolutePath(); }

	QString homeDir() const { return QDir::home().absolutePath(); }

    QVariantMap appInfo() const { 
        return ctx_.GetAppInfo() ? ctx_.GetAppInfo()->ToVariantMap()
                                 : QVariantMap();
    }

    // IMPORTANT: this sets the paths associated with a specific resource tag
    // e.g. "images" "$home/images"
    // allows to specify filenames as "images:mypicture.jpg" which gets
    // translated to "$home/images/mypicture.jpg"
    void setSearchPaths( const QString& prefix, const QStringList& paths ) {
        QDir::setSearchPaths( prefix, paths );
    }
     
    QStringList searchPaths( const QString& prefix ) { return QDir::searchPaths( prefix ); }

    QVariant eval( QString code, const QStringList& filters = QStringList() ) { 
        return ctx_.Eval( code, filters );
    }

    QVariant loadObject( const QString& uri,  //used as a regular file path for now
                         bool persistent = false ) { return ctx_.LoadObject( uri, persistent ); }
    
    QString filter( QString code, const QStringList& filterIds = QStringList() ) {
        return ctx_.Filter( code, filterIds );
    }
#ifdef LOCO_GUI
    int eventLoop() { return ctx_.Exec(); }
#endif
    
    QVariant evalFile( const QString& uri, const QStringList& filterIds = QStringList() ) {
		return ctx_.Include( uri, filterIds );
	}

    void loadFilter( const QString& id, const QString& uri ) {
        ctx_.LoadFilter( id, uri );
    }

    bool hasFilter( const QString& id ) { return ctx_.HasFilter( id ); }

    void addScriptFilter( const QString& id,
                          const QString& jfun,
						  const QString& jcode = "", 
                          const QString& jerrfun = "",
                          const QString& codePlaceHolder = "" ) {
        ctx_.AddScriptFilter( id, jfun, jcode, jerrfun, codePlaceHolder );
    }

	void loadScriptFilter( const QString& id,
		                   const QString& uri,
                           const QString& jfun,
                           const QString& jerrfun = "",
                           const QString& codePlaceHolder = "" ) {
        ctx_.LoadScriptFilter( id, uri, jfun, jerrfun, codePlaceHolder );
    }

    QStringList cmdLine() const { return ctx_.Cmdline(); }
    
    QString env( const QString& envVarName ) const {
#if !defined( Q_WS_WIN )
        return ::getenv( envVarName.toAscii().constData() );
#else
        char* p = 0;
        size_t len = 0;
        errno_t err = _dupenv_s( &p, &len, envVarName.toStdString().c_str() );
        if( err ) return "";
        QString ret( p );
        free( p );
        return ret;
#endif
    }
    
    void registerErrCBack( const QString& code, const QStringList& filterIds = QStringList() ) { 
        ctx_.RegisterJSErrCBack( code, filterIds );
    }

    
    QString qtVersion() const { return qVersion(); }

    //QString qtWebkitVersion() { return ::qtWebkitVersion(); }


    QString os() const {
        #if defined( Q_WS_X11 )
            return "UNIX";
        #elif defined( Q_WS_WIN )
            return "WINDOWS";
        #elif defined( Q_WS_MAC )
            return "MAC"
        #else
            return "";
        #endif                 
    }

    QVariantMap env() const {
        QProcessEnvironment pe = QProcessEnvironment::systemEnvironment();
        QStringList e = pe.toStringList();
        QVariantMap vm;
        for( QStringList::const_iterator i = e.begin(); i != e.end(); ++i ) {
            QStringList kv = i->split( "=" );
            vm[ *kv.begin() ] = *( ++kv.begin() );
        }
        return vm;          
    }

private slots:
    //forward errors received from Context,
    friend class Context; 
    void ForwardError( const QString& err ) { error( err ); }
     

private:
    Context& ctx_;

};




}
