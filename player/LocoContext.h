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
#include <QProcessMap>

#include <cstdlib>

#include "EWL.h"
#include "LocoFactory.h"
#include "LocoFilter.h"



namespace loco {

typedef QVariantMap CMDLine;

typedef QList< Factory* > Factories;
typedef QMap< QString, Filter*  > Filters;
typedef QList< QPluginLoader* > PluginLoaders;
typedef QList< Object* > JScriptObjCtxInstances; 


class Context : public Object {
    
    Q_OBJECT
    
    Context( QWebFrame* wf, QApplication* app, const CMDLine& cmdLine,
             Context* parent = 0 ) : Object( "LocoContext", "Loco/Context" ),
        webFrame_( wf ), app_( app ), parent_( parent ), cmdLine_( cmdLine ) {
        connect( webFrame_, SIGNAL( javaScriptWindowObjectCleared() ),
                 this, SLOT( RemoveInstanceObjects() ) );
        connect( webFrame_, SIGNAL( javaScriptWindowObjectCleared() ),
                 this, SLOT( RemoveFilters() ) );
        connect( webFrame_, SIGNAL( javaScriptWindowObjectCleared() ),
                 this, SLOT( AddJavaScriptObjects() ) );

        connect( this, SIGNAL( destroyed() ), this, SLOT( RemoveInstanceObjects() ) );
        connect( this, SIGNAL( destroyed() ), this, SLOT( RemoveStdObjects() ) );
        connect( this, SIGNAL( destroyed() ), this, SLOT( RemoveFilters() ) );
        
        AddJSStdOBject( this );
        AddJSStdObject( &console );     
        ///@todo should we call AddJavaScriptObjects() here ?
        //AddJavaScriptObjects();
    }
public:

    // object ownership is transferred to context 

    void AddJSStdObject( Object* obj ) { jscriptStdObjects_.push_back( obj ); }
    
    void AddJSCtxObject( Object* obj ) { jscriptCtxInstances_.push_back( obj ); }

    void AddFilter( const QString& id, Filter* f ) { filters_[ id ] = f; }
            
public slots:
    /// Add javascript objects that need to be available at initialization time
    void AddJavascriptObjects() {
        for( JScriptObjCtxInstances::const_iterator i = jscriptStdObjects_.begin();
            i != jscriptStdObjects_.end(); ++i ) {
            webFrame_->addToJavascriptWindowObject( i->jsInstanceName(), &(*i) );  
        }
        ///@todo sjould we add parent's context std objects or parent's instance objects ?
        /// Parent()->jscriptsStdObjects()
    }

    /// Remove instance objects created during context operations
    void RemoveInstanceObjects() {
        for( JScriptObjCtxInstances::iterator i = jscriptCtxInstances_.begin();
            i != jscriptCtxInstances_.end(); ++i ) {
            i->destroy();
        }
        for( Factories::iterator i = ctxFactories_.begin();
            i != ctxFactories_.end(); ++i ) {
            i->deleteLater();
        }
        for( PluginLoaders::iterator i = ctxPluginLoaders_.begin();
            i != ctxPluginLoaders_.end(); ++i ) {
            i->unload();
            i->deleteLater();
        }
       
        jscriptCtxInstances_.clear();
        ctxfactories_.clear();
        ctxPluginLoaders_.clear();
    }

    void RemoveStdObjects() {
        for( JScriptObjCtxInstances::iterator i = jscriptStdObj_.begin();
            i != jscriptStdObj_.end(); ++i ) {
            i->destroy();
        }
        for( Factories::iterator i = stdFactories_.begin();
            i != stdFactories_.end(); ++i ) {
            i->deleteLater();
        }
        for( PluginLoaders::iterator i = stdPluginLoaders_.begin();
            i != stdPluginLoaders_.end(); ++i ) {
            i->unload();
            i->deleteLater();
        }
       
        jscriptStdObjects_.clear();
        stdFactories_.clear();
        stdPluginLoaders_.clear();
    }    

   void RemoveFilters() {
        for( Filters::iterator i = filters_.begin();
            i != filters_.end(); ++i ) {
            i->second->deleteLater();
        }
        filters_.clear();
        for( PluginLoaders::iterator i = filterPluginLoaders_.begin();
             i != filterPluginLoaders_.end(); ++i ) {
            i->unload();
            i->deleteLater();
        } 
        filterPluginLoaders_.clear(); 
   }  

public slots: // js interface
    //void require( const QString& path, const QStringList& filters = QStringList() );
    //QString read( const QString& path, const QStringList& filters = QStringList() );
    
    QVariant eval( QString code, const QStringList& filters = QStringList() ) {
    	code = filter( code, filters );
    	if( !error() ) return wf_->evaluateJavaScript( code );
    	else {
            wf_->evaluateJavaScript( errCBak_ );
            return QVariant();
        } 
    }
    
    QString filter( QString code, const QStringList& filterIds = QStringList() ) {
        for( QStringList::const_iterator f = filterIds.begin();
             f != filterIds.end(); ++f ) { 
            Filters::iterator i = filters_.find( *f );
            if( i != filters_.end() ) {
                Filter* fp =  i->second;
                code = fp->Filter( code );
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
            webFrame_->evaluateJavaScript( errCBack_ );
            return QVariant();
        } else {
            loco::Factory* lf = qobject_cast< loco::Factory* >( pl->instance() );
            if( lf == 0 ) {
                error( "Wrong plugin type" );
                webFrame_->evaluateJavaScript( errCBack_ );    
            }
            lf->SetContext( this );
            if( persist ) {
                stdPluginLoaders_.push_back( pl );
                stdFactories_.push_back( lf );
            } else {
                ctxPluginLoaders_.push_back( pl );
                ctxFactories_.push_back( lf );
            }
            webFrame_->addToJavascriptWindowObject( lf->jsInstanceName(), lf );
            return webFrame_->evaluateJavaScript( lf->jsInstanceName() ); 
        }     
    }

    void addFilter( const QString& id, const QString& uri ) {
        QPluginLoader* pl = new QPluginLoader( uri );
        if( !pl->load() ) {
            error( pl->errorString() );
            webFrame_->evaluateJavaScript( errCBack_ );
        } else {
            loco::Filter* lf = qobject_cast< loco::Filter* >( pl->instance() );
            if( lf == 0 ) {
                error( "Wrong filter type" );
                webFrame_->evaluateJavaScript( errCBack_ );    
            }
            filters_[ id ] = lf;
            filterPluginLoaders_.push_back( pl );
        }      
    }

    bool hasFilter( const QString& id ) { return filters_.find( id ) != filters_.end(); }

    bool addScriptFilter( const QString& js,
                          const QString& id,
                          const QString& jfun,
                          const QString& jerrfun = "",
                          const QString& codePlaceHolder = "",
                          const QStringList& filtersOnFilter = QStringList() ) {
        if( !filtersOnFilter.empty() ) {
            QString c = js;
            for( QStringList::iterator i = filters_.begin(); i != filters_.end() ) {
                c = i->Filter( c );
                if( i->error() ) {
                    error( i->getLastError() );
                    break;
                }
            }
            if( error() ) return false;
            filters_[ id ] = new ScriptFilter( wf_, c, jfun, jerrfun, codePlaceHolder );
        } else filters_[ id ] = new ScriptFilter( wf_, js );
        return true;     
    }

    QVariantMap cmdLine() const { return cmdLine_; }
    
    QString env( const QString& envVarName ) const {
        return ::getenv( envVarName.toAscii().constData() );
    }
    
    void registerErrCBack( const QString& code ) { jsErrCBack_ = code; }

    QString system( const QString& program,
                    const QStringList& args,
                    const QVariantMap& env,
                    int timeout = 10000 ) {
        QProcessMap pm;
        for( QVariantMap::const_iterator i = env.begin();
             i != env.end(); ++i ) {
            const QString& envVar = i->first;
            if( envVar.size() == 0 ) continue;
            const QVariant& v = i->second;
            if( v.isNull() || !v.isValid() || v.toString().size() = 0 ) continue;
            pm.insert( i->first, v.toString() );
        }
        QProcess p;
        p.setProcessEnvironment( pm );
        p.start( program, args );
        const bool wff = p.waitForFinished( timeout );
        if( !wff ) {
            switch( p.error() ) {
            case QProcess::FailedToStart: error( "QProcess: FailedToStart" );
                                          break;
            case QProcess::Crashed:       error( "QProcess: Crashed" );
                                          break;
            case QProcess::Timedout:      error( "QProcess: Timedout" );
                                          break;
            case QProcess::WriteError:    error( "QProcess: WriteError" );
                                          break;
            case QProcess::ReadError:     error( "QProcess: ReadError" );
                                          break;
            case QProcess::UnknownError:  error( "QProcess: UnknownError" );
                                          break;
            default: error( "Unknown QProcess::start error" );
                     break;
            }
            wf_->evaluateJavaScript( errCBack_ );
            return "";
        } else {
            QString output = p.readAllStandardOutput();
            if( output.size() == 0 ) output = p.readAllStandardError();
            return output;
        }                          
    }

    QVariantMap env() {
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
    JScriptObjCtxInstances jscriptStdObjects_;
    JScriptObjCtxInstances jscriptCtxObjects_;
    QString jsErrBack_;
};

}
