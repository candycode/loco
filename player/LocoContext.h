#pragma once
//#SRCHEADER

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

#include <cstdlib>

#include "EWL.h"
#include "LocoFilter.h"
#include "LocoObject.h"
#include "LocoConsole.h"
#include "LocoScriptFilter.h"
#include "LocoFileSystem.h"


namespace loco {

typedef QVariantMap CMDLine;

typedef QMap< QString, Filter*  > Filters;
typedef QList< QPluginLoader* > PluginLoaders;
typedef QList< Object* > JScriptObjCtxInstances; 

typedef QMap< bool, QMap< QString, Object* > > URIObjectMap;
typedef QMap< QString, Filter* > URIFilterMap;

class Context : public Object {
    Q_OBJECT

public:    
    Context( QWebFrame* wf, QApplication* app, const CMDLine& cmdLine,
             Context* parent = 0 ) : Object( 0, "LocoContext", "Loco/Context" ),
        webFrame_( wf ), app_( app ), parent_( parent ), cmdLine_( cmdLine ) {
        connect( webFrame_, SIGNAL( javaScriptWindowObjectCleared() ),
                 this, SLOT( RemoveInstanceObjects() ) );
        connect( webFrame_, SIGNAL( javaScriptWindowObjectCleared() ),
                 this, SLOT( RemoveFilters() ) );
        connect( webFrame_, SIGNAL( javaScriptWindowObjectCleared() ),
                 this, SLOT( AddJavaScriptObjects() ) );
        connect( webFrame_, SIGNAL( javaScriptWindowObjectCleared() ),
                 this, SLOT( InitJScript() ) );

        connect( this, SIGNAL( destroyed() ), this, SLOT( RemoveInstanceObjects() ) );
        connect( this, SIGNAL( destroyed() ), this, SLOT( RemoveStdObjects() ) );
        connect( this, SIGNAL( destroyed() ), this, SLOT( RemoveFilters() ) );
        
        AddJSStdObject( this );
        fileMgr_.SetContext( this );
        AddJSStdObject( &fileMgr_ );
        AddJSStdObject( &console_ );     
        ///@todo should we call AddJavaScriptObjects() here ?
        //AddJavaScriptObjects();
    }
// called from C++
public:

    // object ownership is transferred to context 

    void AddJSStdObject( Object* obj ) { jscriptStdObjects_.push_back( obj ); }
    
    void AddJSCtxObject( Object* obj ) { jscriptCtxInstances_.push_back( obj ); }

    // call this method from factory objects willing to add new objects into
    // the current context and have the object's lifetime managed by javascript
    // NEVER add root objects from plugins because such objects must be deleted
    // through the plusgin loader's unload method
    QVariant AddObjToJSContext( Object* obj ) {
        webFrame_->addToJavaScriptWindowObject( obj->jsInstanceName(),
                                                obj,
                                                QScriptEngine::ScriptOwnership );
        connect( obj, SIGNAL( RiseError( Object* ) ), this, SLOT( OnObjectError( Object* ) ) );
        return webFrame_->evaluateJavaScript( obj->jsInstanceName() );
    }

    void AddFilter( const QString& id, Filter* f ) { filters_[ id ] = f; }

// attched to internal signals            
private slots:

    /// 
    void InitJScript() {
        QStringList initCode;
        static const QString GL = "Loco";
        initCode << "var " << GL << " = {}\n"
                 << GL << ".ctx = " + this->jsInstanceName() + ";\n"
                 << GL << ".console = " + console_.jsInstanceName() + ";\n"
                 << GL << ".fs = " + fileMgr_.jsInstanceName() + ";\n"
				 << GL << ".errcback = function() {\n"
				 <<       "  throw 'LocoException: ' + this.ctx.lastError();\n}"; 
		jsErrCBack_ = GL + ".errcback();";
        std::cout << initCode.join("").toStdString() << std::endl << std::endl;
        std::cout << jsErrCBack_.toStdString() << std::endl;
        webFrame_->evaluateJavaScript( initCode.join( "" ) );
    }

    /// this slot can be called from child contexts to make objects
    /// in the parent context available within the child context
    void AddJSStdObjects( QWebFrame* wf ) {
        for( JScriptObjCtxInstances::const_iterator i = jscriptStdObjects_.begin();
            i != jscriptStdObjects_.end(); ++i ) {
            connect( *i, SIGNAL( RiseError( Object* ) ), this, SLOT( OnObjectError( Object* ) ) );
            wf->addToJavaScriptWindowObject( (*i)->jsInstanceName(), *i );  
        } 
    }

    void AddJSCtxObjects( QWebFrame* wf ) {
        for( JScriptObjCtxInstances::const_iterator i = jscriptCtxInstances_.begin();
            i != jscriptCtxInstances_.end(); ++i ) {
            connect( *i, SIGNAL( RiseError( Object* ) ), this, SLOT( OnObjectError( Object* ) ) );
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
    void OnObjectError( Object* obj ) {
        error( obj->jsInstanceName() + ": " + obj->lastError() );
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

    ///\todo properly handle memory
    QVariant loadObject( const QString& uri,  //used as a regular file path for now
                         bool persistent = false ) { 
        if( uriObjectMap_[ persistent ].find( uri ) != uriObjectMap_[ persistent ].end() ) {
            Object* obj = uriObjectMap_[ persistent ][ uri ];
            return webFrame_->evaluateJavaScript( obj->jsInstanceName() );
        }
        QPluginLoader* pl = new QPluginLoader( uri );
		if( !pl->load() ) {
			delete pl;
			error( "Cannot load " + uri );
			webFrame_->evaluateJavaScript( jsErrCBack_ );
			return QVariant();
		} 			
        Object* obj = qobject_cast< ::loco::Object* >( pl->instance() );
        if( !obj ) {
			delete pl;
			error( "Wrong object type " + uri );
			webFrame_->evaluateJavaScript( jsErrCBack_ );
			return QVariant();
		}
		obj->SetContext( this );
        obj->SetPluginLoader( pl );
        connect( obj, SIGNAL( error( const QString& ) ), this, SLOT( OnError( const QString& ) ) );
		if( persistent ) {
			jscriptStdObjects_.push_back( obj );
            obj->SetContextPointerToThis( &( jscriptStdObjects_.back() ) );
			stdPluginLoaders_.push_back( pl );
		} else {
			jscriptCtxInstances_.push_back( obj );
            obj->SetContextPointerToThis( &( jscriptCtxInstances_.back() ) );  
			ctxPluginLoaders_.push_back( pl );
		}
        uriObjectMap_[ persistent ][ uri ] = obj;
        webFrame_->addToJavaScriptWindowObject( obj->jsInstanceName(), obj );
        return webFrame_->evaluateJavaScript( obj->jsInstanceName() );
    }      
    
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
        return ::getenv( envVarName.toAscii().constData() );
    }
    
    void registerErrCBack( const QString& code ) { jsErrCBack_ = code; }

    QString system( const QString& program,
                    const QStringList& args = QStringList(),
                    const QVariantMap& env  = QVariantMap(),
                    int timeout = 10000 ) const {
        QProcessEnvironment pe;
        for( QVariantMap::const_iterator i = env.begin();
             i != env.end(); ++i ) {
            const QString& envVar = i.key();
            if( envVar.size() == 0 ) continue;
            const QVariant& v = i.value();
            if( v.isNull() || !v.isValid() /*|| v.toString().isEmtpy()*/ ) continue;
            pe.insert( i.key(), v.toString() );
        }
        QProcess p;
        p.setProcessEnvironment( pe );
        p.start( program, args );
        const bool wff = p.waitForFinished( timeout );
        if( !wff ) {
            if( p.error() == QProcess::FailedToStart     ) error( "QProcess: FailedToStart" );
            else if( p.error() == QProcess::Crashed      ) error( "QProcess: Crashed" );
            else if( p.error() == QProcess::Timedout     ) error( "QProcess: Timedout" );
            else if( p.error() == QProcess::WriteError   ) error( "QProcess: WriteError" );
            else if( p.error() == QProcess::ReadError    ) error( "QProcess: ReadError" );
            else if( p.error() == QProcess::UnknownError ) error( "QProcess: UnknownError" );
            else error( "Unknown QProcess::start error" );
            webFrame_->evaluateJavaScript( jsErrCBack_ );
            return "";
        } else {
            QString output = p.readAllStandardOutput();
            if( output.size() == 0 ) output = p.readAllStandardError();
            return output;
        }                          
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

private:
    Console console_;
    QWebFrame* webFrame_;
    QApplication* app_;
    Context* parent_;
    CMDLine cmdLine_;
    FileSystem fileMgr_;

private:
    Filters filters_;
    PluginLoaders stdPluginLoaders_;
    PluginLoaders ctxPluginLoaders_;
    PluginLoaders filterPluginLoaders_;    
    JScriptObjCtxInstances jscriptStdObjects_;
    JScriptObjCtxInstances jscriptCtxInstances_;
    QString jsErrCBack_;
    URIObjectMap uriObjectMap_;
    URIFilterMap uriFilterMap_;
};

}
