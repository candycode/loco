#pragma once
//#SRCHEADER

#include <QtWebKit/QWebFrame>
#include <QApplication>
#include <QString>
#include <QList>
#include <QVariant>
#include <QVariantMap>
#include <QPluginLoader>

#include <cstdlib>

#include "EWL.h"
#include "LocoFactory.h"
#include "LocoFilter.h"


namespace loco {

typedef QVariantMap CMDLine;

class Context : public Object {
    
    Q_OBJECT
    
    Context( QWebFrame* wf, QApplication* app, const CMDLine& cmdLine,
             Context* parent = 0 ) : Object( "Context", "Loco/Context" ),
        webFrame_( wf ), app_( app ), parent_( parent ), cmdLine_( cmdLine ) {
        connect( webFrame_, SIGNAL( javaScriptWindowObjectCleared() ),
                 this, SLOT( RemoveInstaceObjects() ) );
        connect( webFrame_, SIGNAL( javaScriptWindowObjectCleared() ),
                 this, SLOT( AddJavaScriptObjects() ) );
        ///@todo should we call AddJavaScriptObjects() here ?
        //AddJavaScriptObjects();
    }
public:

    void AddJSStdObject( Object* obj ) { jscriptStdObjects_.push_back( obj ); }
    
    void AddJSInstanceObject( Object* obj ) { jscriptInstances_.push_back( obj ); }
            
public slots:
    /// Add javascript objects that need to be available at initialization time
    void AddJavascriptObjects() {
        for( JScriptObjInstances::const_iterator i = jscriptStdObjects_.begin();
            i != jscriptStdObjects_.end(); ++i ) {
            webFrame_->addToJavascriptWindowObject( i->jsInstanceName(), &(*i) );  
        }
    }
    /// Remove instance objects created during context operations
    void RemoveInstanceObjects() {
        for( JScriptObjInstances::const_iterator i = jscriptInstances_.begin();
            i != jscriptInstances_.end(); ++i ) {
            i->destroy();
        }
        jscriptInstances_.clear();
    }
public slots: // js interface
    //void require( const QString& path, const QStringList& filters = QStringList() );
    //QString read( const QString& path, const QStringList& filters = QStringList() );
    
    QVariant eval( QString code, const QStringList& filters = QStringList() ) {
    	code = filter( code, filters );
    	if( !error() ) return wf_->evaluateJavaScript( code );
    	else return QVariant(); 
    }
    
    QString filter( QString code, const QStringList& filterIds = QStringList() ) {
        for( QStringList::const_iterator f = filterIds.begin();
             f != filterIds.end(); ++f ) { 
            Filters::iterator i = filters_.find( *f );
            if( i != filters_.end() ) {
                Filter* fp =  filters_[ f ];
                code = fp->Filter( code );
                if( fp->error() ) error( fp->lastError() );
                break;                 
            }
            else {
            	error( "filter id " + *f + " not found" );
            	break;
            }
        }
        return error() ? "" : text; 
    }

    int exec() { return app_->exec(); }
    
    QVariant factory( const QString& uri ) {
        QPluginLoader pl( uri );
        if( !pl.load() ) {
            error( pl.errorString() );
            webFrame_->evaluateJavaScript( errCBack_ );
            return QVariant();
        } else {
            loco::Factory* lf = qobject_cast< loco::Factory* >( pl.instance() );
            if( lf == 0 ) {
                error( "Wrong plugin type" );
                webFrame_->evaluateJavaScript( errCBack_ );    
            }
            factories_.push_back( lf );
            lf->SetContext( this );
            webFrame_->addToJavascriptWindowObject( lf->jsInstanceName(), lf );
            return webFrame_->evaluateJavaScript( lf->jsInstanceName() ); 
        }     
    }
    void addFilter( const QString& id, const QString& uri ) {
        QPluginLoader pl( uri );
        if( !pl.load() ) {
            error( pl.errorString() );
            webFrame_->evaluateJavaScript( errCBack_ );
            return QVariant();
        } else {
            loco::Filter* lf = qobject_cast< loco::Filter* >( pl.instance() );
            if( lf == 0 ) {
                error( "Wrong filter type" );
                webFrame_->evaluateJavaScript( errCBack_ );    
            }
            filters_[ id ] = lf;
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

private:
    Console console_;
    QWebFrame* webFrame_;
    QApplication* app_;
    Context* parent_;
    CMDLine cmdLine_;
private:
    Filters filters_;
    Factories factories_;
    JScriptObjInstances jscriptStdObjects_;
    JScriptObjInstances jscriptIntances_;
    QString jsErrBack_;
};

}
