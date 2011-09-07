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

#include <cstdlib>

#include "EWL.h"
#include "LocoFactory.h"
#include "LocoFilter.h"
#include "LocoObject.h"
#include "LocoConsole.h"
#include "LocoScriptFilter.h"


namespace loco {

typedef QVariantMap CMDLine;

typedef QList< Factory* > Factories;
typedef QMap< QString, Filter*  > Filters;
typedef QList< QPluginLoader* > PluginLoaders;
typedef QList< Object* > JScriptObjCtxInstances; 


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
        AddJSStdObject( &console_ );     
        ///@todo should we call AddJavaScriptObjects() here ?
        //AddJavaScriptObjects();
    }
public:

    // object ownership is transferred to context 

    void AddJSStdObject( Object* obj ) { jscriptStdObjects_.push_back( obj ); }
    
    void AddJSCtxObject( Object* obj ) { jscriptCtxInstances_.push_back( obj ); }

    void AddFilter( const QString& id, Filter* f ) { filters_[ id ] = f; }
            
public slots:

    /// 
    void InitJScript() {
        QStringList initCode;
        static const QString GL = "Loco";
        initCode << "var " << GL << " = {}\n"
                 << GL << ".ctx = " + this->jsInstanceName() + ";\n"
                 << GL << ".console = " + console_.jsInstanceName() + ";\n";
        webFrame_->evaluateJavaScript( initCode.join( "" ) );
    }

    /// this slot can be called from child contexts to make objects
    /// in the parent context available within the child context
    void AddJSStdObjects( QWebFrame* wf ) {
        for( JScriptObjCtxInstances::const_iterator i = jscriptStdObjects_.begin();
            i != jscriptStdObjects_.end(); ++i ) {
            wf->addToJavaScriptWindowObject( (*i)->jsInstanceName(), *i );  
        }
    }

    void AddJSCtxObjects( QWebFrame* wf ) {
        for( JScriptObjCtxInstances::const_iterator i = jscriptCtxInstances_.begin();
            i != jscriptCtxInstances_.end(); ++i ) {
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
            if( (*i)->GetContext() == this ) (*i)->destroy();
        }
        for( Factories::iterator i = ctxFactories_.begin();
            i != ctxFactories_.end(); ++i ) {
            (*i)->Destroy();
        }
        for( PluginLoaders::iterator i = ctxPluginLoaders_.begin();
            i != ctxPluginLoaders_.end(); ++i ) {
            (*i)->unload();
            (*i)->deleteLater();
        }
       
        jscriptCtxInstances_.clear();
        ctxFactories_.clear();
        ctxPluginLoaders_.clear();
    }

    void RemoveStdObjects() {
        for( JScriptObjCtxInstances::iterator i = jscriptStdObjects_.begin();
            i != jscriptStdObjects_.end(); ++i ) {
            (*i)->destroy();
        }
        for( Factories::iterator i = stdFactories_.begin();
            i != stdFactories_.end(); ++i ) {
            (*i)->Destroy();
        }
        for( PluginLoaders::iterator i = stdPluginLoaders_.begin();
            i != stdPluginLoaders_.end(); ++i ) {
            (*i)->unload();
            (*i)->deleteLater();
        }
       
        jscriptStdObjects_.clear();
        stdFactories_.clear();
        stdPluginLoaders_.clear();
    }    

   void RemoveFilters() {
        for( Filters::iterator i = filters_.begin();
            i != filters_.end(); ++i ) {
            i.value()->deleteLater();
        }
        filters_.clear();
        for( PluginLoaders::iterator i = filterPluginLoaders_.begin();
             i != filterPluginLoaders_.end(); ++i ) {
            (*i)->unload();
            (*i)->deleteLater();
        } 
        filterPluginLoaders_.clear(); 
   }  

public slots: // js interface
    //void require( const QString& path, const QStringList& filters = QStringList() );
    //QString read( const QString& path, const QStringList& filters = QStringList() );
    
    QVariant eval( QString code, const QStringList& filters = QStringList() ) {
    	code = filter( code, filters );
    	if( !error() ) return webFrame_->evaluateJavaScript( code );
    	else {
            webFrame_->evaluateJavaScript( jsErrCBack_ );
            return QVariant();
        } 
    }

    ///\todo properly handle memory
    QVariant loadObject( const QString& uri ) { //used as a regular file path for now
        QPluginLoader* pl = new QPluginLoader( uri );
        if( !pl->load() ) throw std::runtime_error( "Cannot load " + uri.toStdString() );
        Object* obj = qobject_cast< ::loco::Object* >( pl->instance() );
        if( !obj ) throw std::runtime_error( "Not a loco::Object" );
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
    
    QVariant factory( const QString& uri, bool persist ) {
        QPluginLoader* pl = new QPluginLoader( uri );
        if( !pl->load() ) {
            delete pl;
            error( pl->errorString() );
            webFrame_->evaluateJavaScript( jsErrCBack_ );
            return QVariant();
        } else {
            loco::Factory* lf = qobject_cast< loco::Factory* >( pl->instance() );
            if( lf == 0 ) {
                error( "Wrong plugin type" );
                webFrame_->evaluateJavaScript( jsErrCBack_ );    
            }
            lf->SetContext( this );
            if( persist ) {
                stdPluginLoaders_.push_back( pl );
                stdFactories_.push_back( lf );
            } else {
                ctxPluginLoaders_.push_back( pl );
                ctxFactories_.push_back( lf );
            }
            webFrame_->addToJavaScriptWindowObject( lf->jsInstanceName(), lf );
            return webFrame_->evaluateJavaScript( lf->jsInstanceName() ); 
        }     
    }

    void addFilter( const QString& id, const QString& uri ) {
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

private:
    Filters filters_;
    Factories stdFactories_;
    Factories ctxFactories_;     
    PluginLoaders stdPluginLoaders_;
    PluginLoaders ctxPluginLoaders_;
    PluginLoaders filterPluginLoaders_;    
    JScriptObjCtxInstances jscriptStdObjects_;
    JScriptObjCtxInstances jscriptCtxInstances_;
    QString jsErrCBack_;
};

}
