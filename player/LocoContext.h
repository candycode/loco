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

#include <algorithm>
#include <cstdlib>

#include "EWL.h"
#include "LocoFilter.h"
#include "LocoObject.h"
#include "LocoConsole.h"
#include "LocoScriptFilter.h"
#include "LocoFileSystem.h"
#include "LocoSystem.h"


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


class Context : public Object {
    Q_OBJECT  

public:
    Context() : Object( 0, "LocoContext", "Loco/Context" ),
     globalContextJSName_( "Loco" ), jsInitGenerator_( 0 ) {}   
    
    Context( QWebFrame* wf, QApplication* app, const CMDLine& cmdLine,
             Context* parent = 0 );

    ~Context() {
		// remove non-QObject-derived objects; the others are deleted from slots attached to
		// the 'destroyed' signal
		delete jsInitGenerator_;
	}   

    void Init( QWebFrame* wf, QApplication* app = 0, const CMDLine& cmdLine = CMDLine(), Context* parent = 0 );
// called from C++
public:

    // object ownership is transferred to context 

    void AddJSStdObject( Object* obj, bool immediateAdd = false ) { 
        jscriptStdObjects_.push_back( obj );
        if( obj->GetContext() == 0 ) obj->SetContext( this );
        if( obj->GetPluginLoader() == 0 && obj->parent() == 0 && obj != this ) obj->setParent( this );
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
        jsErrCBack_ = filter( code, filterIds );
    }

    const QString& GetJSErrCBack() const { return jsErrCBack_; }

    void SetJSInitGenerator( IJavaScriptInit* jsi ) {
        if( jsi == jsInitGenerator_ ) return;
        delete jsInitGenerator_;
        jsInitGenerator_ = jsi;
    }

    IJavaScriptInit* GetJSInitGenerator() const { return jsInitGenerator_; }

	const JScriptObjCtxInstances& GetStdJSObjects() const { return jscriptStdObjects_; }

// attched to internal signals            
private slots:

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

// invocable from javascript
public slots: // js interface
    
	//void require( const QString& path, const QStringList& filters = QStringList() );
    
	//QString read( const QString& path, const QStringList& filters = QStringList() ) {
	//}
    
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
    	code = filter( code, filters );
    	if( !error() ) return webFrame_->evaluateJavaScript( code );
    	else {
            webFrame_->evaluateJavaScript( jsErrCBack_ );
            return QVariant();
        } 
    }

    QVariant loadObject( const QString& uri,  //used as a regular file path for now
                         bool persistent = false );
    
    QString filter( QString code, const QStringList& filterIds = QStringList() ) {
        for( QStringList::const_iterator f = filterIds.begin();
             f != filterIds.end(); ++f ) { 
            Filters::iterator i = filters_.find( *f );
            if( i != filters_.end() ) {
                Filter* fp =  i.value();
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

    int exec() { return app_->exec(); }
    

    void addFilter( const QString& id, const QString& uri ) {
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

    bool hasFilter( const QString& id ) { return filters_.find( id ) != filters_.end(); }

    void addScriptFilter( const QString& id,
                          const QString& jfun,
                          const QString& jerrfun = "",
                          const QString& codePlaceHolder = "" ) {
        filters_[ id ] = new ScriptFilter( webFrame_, jfun, jerrfun, codePlaceHolder );
    }

    QVariantMap cmdLine() const { return cmdLine_; }
    
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
    
    void registerErrCBack( const QString& code ) { jsErrCBack_ = code; }

    
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

private:
    
    void ConnectErrCBack( Object* obj ) {
       if( obj == this ) return;  
       connect( obj, SIGNAL( onError( const QString& ) ), this, SLOT( OnObjectError( const QString& ) ) );
    }

private:
    Console console_;
    QWebFrame* webFrame_;
    QApplication* app_;
    Context* parent_;
    CMDLine cmdLine_;
    FileSystem fileMgr_;
    System system_;

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
    
};
}
