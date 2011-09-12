#pragma once
//#SRCHEADER

#include <cstdlib> //for dupenv (win), putenv, getenv

#include <QtWebKit/QWebFrame>
#include <QApplication>
#include <QString>
#include <QList>
#include <QVariant>
#include <QVariantMap>
#include <QPluginLoader>
#include <QProcess>
#include <QProcessEnvironment>
#include <QDir>
#include <QPointer>
#include <QtNetwork/QNetworkAccessManager>
#include <QSet>
#include <QUrl>

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


namespace loco {

typedef QVariantMap CMDLine;

typedef QMap< QString, Filter*  > Filters;
typedef QList< QPluginLoader* > PluginLoaders;
typedef QList< Object* > JScriptObjCtxInstances; 

typedef QMap< bool, QMap< QString, Object* > > URIObjectMap;
typedef QMap< QString, Filter* > URIFilterMap;

struct IJavaScriptInit {
    virtual void SetContext( Context*  ) = 0;
    virtual QString GenerateCode() const = 0;
    virtual ~IJavaScriptInit() = 0 {}    
};


class Context : public EWL {
    Q_OBJECT  

public:
    
    Context();
   
    Context( QWebFrame* wf, QApplication* app, const CMDLine& cmdLine,
             Context* parent = 0 );

    ~Context(); 

    void Init( QWebFrame* wf, QApplication* app = 0, const CMDLine& cmdLine = CMDLine(), Context* parent = 0 );
// called from C++
public:

    // object ownership is transferred to context 
    
    void AddContextToJS(); 

    void AddJSStdObject( Object* obj, bool immediateAdd = false ) { 
        jscriptStdObjects_.push_back( obj );
        if( obj->GetContext() == 0 ) obj->SetContext( this );
        if( obj->GetPluginLoader() == 0 && obj->parent() == 0 ) obj->setParent( this );
        if( immediateAdd ) {
            webFrame_->addToJavaScriptWindowObject( obj->jsInstanceName(), obj );
        }
    }
    
    void AddJSCtxObject( Object* obj, bool immediateAdd = false ) {
        jscriptCtxInstances_.push_back( obj );
        if( immediateAdd ) {
            if( obj->GetContext() == 0 ) obj->SetContext( this );
            webFrame_->addToJavaScriptWindowObject( obj->jsInstanceName(), obj );
        }
        if( obj->GetPluginLoader() == 0 && obj->parent() == 0 ) obj->setParent( this );
    }

    void RemoveJSCtxObject( Object* obj ) {
        JScriptObjCtxInstances::iterator i = 
            std::find( jscriptCtxInstances_.begin(),  jscriptCtxInstances_.end(), obj );
        if( i != jscriptCtxInstances_.end() ) jscriptCtxInstances_.erase( i );
    }

    // call this method from factory objects willing to add new objects into
    // the current context and have the object's lifetime managed by javascript
    // NEVER add root objects from plugins because such objects must be deleted
    // through the plusgin loader's unload method
    QVariant AddObjToJSContext( Object* obj ) {
        webFrame_->addToJavaScriptWindowObject( obj->jsInstanceName(),
                                                obj,
                                                QScriptEngine::ScriptOwnership );
        obj->SetContext( this );
        ConnectErrCBack( obj );
        instanceObjs_.push_back( obj );
        return webFrame_->evaluateJavaScript( obj->jsInstanceName() );
    }

    void AddFilter( const QString& id, Filter* f ) { 
        if( f->GetPluginLoader() && f->parent() == 0 ) f->setParent( this );
        filters_[ id ] = f;
    }

    Object* Find( const QString& jsInstanceName ) const;

    void SetJSGlobalNameForContext( const QString& n ) {
        globalContextJSName_ = n;
    }

    const QString& GetJSGlobalNameForContext() const {
        return globalContextJSName_;
    }

	void SetJSInitializer( IJavaScriptInit* jsi ) { jsInitGenerator_ = jsi; } 

	IJavaScriptInit* GetJSInitializer() const { return jsInitGenerator_; }
      
    void SetJSErrCBack( const QString& code,
                        const QStringList& filterIds = QStringList() ) {
        jsErrCBack_ = Filter( code, filterIds );
    }

    const QString& GetJSErrCBack() const { return jsErrCBack_; }

    void SetJSInitGenerator( IJavaScriptInit* jsi ) {
        if( jsi == jsInitGenerator_ ) return;
        delete jsInitGenerator_;
        jsInitGenerator_ = jsi;
    }

    IJavaScriptInit* GetJSInitGenerator() const { return jsInitGenerator_; }

	const JScriptObjCtxInstances& GetStdJSObjects() const { return jscriptStdObjects_; }

    void SetNetworkAccessManager( NetworkAccessManager* nam ) { 
        if( nam == netAccessMgr_ && nam != 0 ) return;
        if( netAccessMgr_ != 0 ) {
            disconnect( netAccessMgr_, SIGNAL( NetworkRequestDenied( QString ) ),
                        this, SLOT( OnNetworkRequestDenied( QString ) ) );
            disconnect( netAccessMgr_, SIGNAL( UnauthorizedNetworkAccessAttempt() ),
                        this, SLOT( OnUnauthorizedNetworkAccess() ) );
        }
        netAccessMgr_ = nam;
        if( netAccessMgr_ ) {
            connect( netAccessMgr_, SIGNAL( NetworkRequestDenied( QString ) ),
                     this, SLOT( OnNetworkRequestDenied( QString ) ) );
            connect( netAccessMgr_, SIGNAL( UnauthorizedNetworkAccessAttempt() ),
                     this, SLOT( OnUnauthorizedNetworkAccess() ) ); 
        }
    }

    const QNetworkAccessManager* GetNetworkAccessManager( QNetworkAccessManager* nam ) const { return netAccessMgr_; }

    void SetFileAccessManager( FileAccessManager* fm ) { fileAccessMgr_ = fm; }

    const FileAccessManager* GetFileAccessManager() const { return fileAccessMgr_; }

    void SetMaxRedirections( int r ) { maxNetRedirections_ = r; }

    int GetMaxRedirection() const { return maxNetRedirections_; }

    void SetNetReadTimeout( int ms ) { readNetworkTimeout_ = ms; }

    int GetNetReadTimeout() const { return readNetworkTimeout_; }

// attched to internal signals            
private slots:

    void OnUnauthorizedNetworkAccess() {
        error( "Unauthorized network access attempted" );
    }
    
    void OnNetworkRequestDenied( QString url ) {
        error( "Unauthorized access to " + url + " attempted" ); 
    }
    
    /// 
    void InitJScript() {
        if( jsInitCode_.isEmpty() ) jsInitCode_ = jsInitGenerator_->GenerateCode();   
        webFrame_->evaluateJavaScript( jsInitCode_ );
    }

    /// this slot can be called from child contexts to make objects
    /// in the parent context available within the child context
    void AddJSStdObjects( QWebFrame* wf ) {
        for( JScriptObjCtxInstances::const_iterator i = jscriptStdObjects_.begin();
             i != jscriptStdObjects_.end(); ++i ) {
            if( (*i)->GetContext() == 0 ) (*i)->SetContext( this );      
            ConnectErrCBack( *i );
            if( (*i)->GetPluginLoader() == 0 && (*i)->parent() == 0 ) (*i)->setParent( this );
            wf->addToJavaScriptWindowObject( (*i)->jsInstanceName(), *i );  
        } 
    }

    void AddJSCtxObjects( QWebFrame* wf ) {
        for( JScriptObjCtxInstances::const_iterator i = jscriptCtxInstances_.begin();
             i != jscriptCtxInstances_.end(); ++i ) {
            if( (*i)->GetContext() == 0 ) (*i)->SetContext( this );
            ConnectErrCBack( *i );
             if( (*i)->GetPluginLoader() == 0 && (*i)->parent() == 0 ) (*i)->setParent( this );
            wf->addToJavaScriptWindowObject( (*i)->jsInstanceName(), *i );  
        }
    }

    /// Add javascript objects that need to be available at initialization time
    void AddJavaScriptObjects() {
        AddJSStdObjects( webFrame_ );
        ///@todo sjould we add parent's context std objects or parent's instance objects ?
        /// Parent()->jscriptsStdObjects()
    }

    /// Remove instance objects created during context operations
    void RemoveInstanceObjects() {
        for( JScriptObjCtxInstances::iterator i = jscriptCtxInstances_.begin();
            i != jscriptCtxInstances_.end(); ++i ) {
            if( (*i) == 0 ) continue;
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
            if( (*i) == 0 ) continue;
            if( (*i)->GetContext() == this ) (*i)->destroy();
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
    }
	

public slots:
    // loco::Objects should be connected to this slot to have errors handled by the context
    // and indirectly by javascript
    // an object RaiseError signal must be connected to one and only one context instance
    void OnObjectError( const QString& err ) {
        error( err );
        webFrame_->evaluateJavaScript( jsErrCBack_ ); 
    } 

private:
    
    void ConnectErrCBack( Object* obj );

private: //js interface invoked from JSContext

friend class JSContext;

public: 
    // Eval needs to be accessible for starting script execution
    QVariant Eval( QString code, const QStringList& filters = QStringList() ) { 
       code = Filter( code, filters );
       if( !error() ) return webFrame_->evaluateJavaScript( code );
       else {
           webFrame_->evaluateJavaScript( jsErrCBack_ );
           return QVariant();
       } 
    }
private:

    QByteArray Filter( QByteArray data, const QStringList& filterIds = QStringList() ) {
        for( QStringList::const_iterator f = filterIds.begin();
             f != filterIds.end(); ++f ) { 
            Filters::iterator i = filters_.find( *f );
            if( i != filters_.end() ) {
                ::loco::Filter* fp =  i.value();
                data = fp->Apply( data );
                if( fp->error() ) error( fp->lastError() );
                break;                 
            }
            else {
    	        error( "filter id " + *f + " not found" );
    	        break;
            }
        }
        return error() ? QByteArray() : data; 
    }

    QString Filter( QString code, const QStringList& filterIds = QStringList() ) {
        for( QStringList::const_iterator f = filterIds.begin();
             f != filterIds.end(); ++f ) { 
            Filters::iterator i = filters_.find( *f );
            if( i != filters_.end() ) {
                ::loco::Filter* fp =  i.value();
                code = fp->Apply( code );
                if( fp->error() ) error( fp->lastError() );
                break;                 
            }
            else {
    	        error( "filter id " + *f + " not found" );
    	        break;
            }
        }
        return error() ? "" : code; 
    }

    QVariant LoadObject( const QString& uri,  //used as a regular file path for now
                         bool persistent = false );

    int Exec() { return app_->exec(); }

    void AddFilter( const QString& id, const QString& uri ) {
        if( uriFilterMap_.find( uri ) != uriFilterMap_.end() ) return;
        QPluginLoader* pl = new QPluginLoader( uri );
        if( !pl->load() ) {
            error( pl->errorString() );
            webFrame_->evaluateJavaScript( jsErrCBack_ );
        } else {
            loco::Filter* lf = qobject_cast< loco::Filter* >( pl->instance() );
            if( lf == 0 ) {
                error( "Wrong filter type" );
                webFrame_->evaluateJavaScript( jsErrCBack_ );    
            }
            filters_[ id ] = lf;
            filterPluginLoaders_.push_back( pl );
            uriFilterMap_[ uri ] = lf;
        }      
    }
    
    bool HasFilter( const QString& id ) { return filters_.find( id ) != filters_.end(); }

    void AddScriptFilter( const QString& id,
                          const QString& jfun,
                          const QString& jerrfun = "",
                          const QString& codePlaceHolder = "" ) {
        filters_[ id ] = new ScriptFilter( webFrame_, jfun, jerrfun, codePlaceHolder );
    }

    QVariantMap Cmdline() const { return cmdLine_; }

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
        QString code = Read( uri, filters );
        if( code.isEmpty() ) return false;
        return Eval( code );
    }

    QVariant Require( QString uri, const QStringList& filters = QStringList() ) {
        if( !uri.startsWith( ":" ) && !uri.contains( "://" ) ) {
            QDir d;
            uri = d.absoluteFilePath( uri );    
        } 
        if( requireSet_.find( uri ) != requireSet_.end() ) return QVariant(); 
        QString code = Read( uri, filters );
        if( code.isEmpty() ) return false;
        requireSet_.insert( uri );
        return Eval( code );
    } 

private:
    QByteArray ReadUrl( const QString& url, QSet< QUrl > redirects = QSet< QUrl >() );

    QByteArray ReadFile( const QString& f );

    void jsErr() { webFrame_->evaluateJavaScript( jsErrCBack_ ); }

private:
    JSContext* jsContext_;
    QWebFrame* webFrame_;
    QApplication* app_;
    Context* parent_;
    CMDLine cmdLine_;
   
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
private: 
    IJavaScriptInit* jsInitGenerator_; 
    NetworkAccessManager* netAccessMgr_;
    FileAccessManager* fileAccessMgr_;
private:
    QSet< QString > requireSet_;
private:
    int readNetworkTimeout_;
    int maxNetRedirections_;        
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
    
	QVariant require( const QString& path, const QStringList& filters = QStringList() ) {
        return ctx_.Require( path, filters );
    }

    QVariant insert( const QString& path, const QStringList& filters = QStringList() ) {
        return ctx_.Insert( path, filters );
    }
    
	QString read( const QString& path, const QStringList& filters = QStringList() ) {
	    return ctx_.Read( path, filters );
    }
    
	QStringList pluginPath() const { return QApplication::libraryPaths(); }

	void setPluginPath( const QStringList& paths ) { QApplication::setLibraryPaths( paths ); }

	QString appFilePath() const { return QApplication::applicationFilePath(); }

	QString appDirPath() const { return QApplication::applicationDirPath(); }

	QString appName() const { return QApplication::applicationName(); }

	QString appVersion() const { return QApplication::applicationVersion(); }

	QString appVendor() const { return QApplication::organizationName(); }

	QString currentDir() const { return QDir::current().absolutePath(); }

	QString homeDir() const { return QDir::home().absolutePath(); }

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

    int exec() { return ctx_.Exec(); }
    

    void addFilter( const QString& id, const QString& uri ) {
        ctx_.AddFilter( id, uri );
    }

    bool hasFilter( const QString& id ) { return ctx_.HasFilter( id ); }

    void addScriptFilter( const QString& id,
                          const QString& jfun,
                          const QString& jerrfun = "",
                          const QString& codePlaceHolder = "" ) {
        ctx_.AddScriptFilter( id, jfun, jerrfun, codePlaceHolder );
    }

    QVariantMap cmdLine() const { return ctx_.Cmdline(); }
    
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
