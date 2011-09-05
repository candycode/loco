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

class Context : QObject, EWL {
    Q_OBJECT
	Context( QWebFrame* wf, QApplication* app, const CMDLine& cmdLine,
	         Context* parent = 0 ) :
	    webFrame_( wf ), app_( app ), parent_( parent ), cmdLine_( cmdLine ) {
        connect( webFrame_, SIGNAL( javaScriptWindowObjectCleared() ),
			     this, SLOT( RemoveInstaceObjects() ) );
		connect( webFrame_, SIGNAL( javaScriptWindowObjectCleared() ),
			     this, SLOT( AddJavaScriptObjects() ) );
		AddJavaScriptObjects();
	}    
public slots:
	/// Add javascript objects that need to be available at initialization time
	void AddJavascriptObjects() {
		for( JScriptObjInstances::const_iterator i = jscriptStdObjects_.begin();
			i != jscriptStdObjects_.end(); ++i ) {
		    webFrame_->addToJavascriptWindowObject( i->first, i->second );  
		}
	}
	/// Remove instance objects created during context operations
	void RemoveInstanceObjects() {
		for( JScriptObjInstances::const_iterator i = jscriptInstances_.begin();
			i != jscriptInstances_.end(); ++i ) {
            factoryInstanceMap_[ i->first ]->destroy( i->second );
		}
		jscriptInstances_.clear();
		jsInstances_ = 0;
	}
public slots: // js interface
    //void require( const QString& path, const QStringList& filters = QStringList() );
	//QString read( const QString& path, const QStringList& filters = QStringList() );
	//QString eval( const QString& code, const QStringList& filters = QStringList() );
	QString filter( const QString& code, const QStringList& filterIds = QStringList() ) {
        bool err = false;
		for( QStringList::const_iterator f = filterIds.begin();
             f != filterIds.end(); ++f ) { 
            Filters::iterator i = filters_.find( *f );
            if( i == filterMap_.end() ) {
                Filter* fp =  filters_[ f ];
                text = fp->Filter( text );
                if( fp->Error() ) error( fp->lastError() );
                err = true;
				break;                 
            }
        }
		return err ? "" : text; 
	}
	int exec() { return app_->exec(); }
	QVariant factory( const QString& uri ) {
	    QPluginLoader pl( uri );
		if( !pl.load() ) {
			Error( pl.errorString() );
            webFrame_->evaluateJavaScript( errCBack_ );
			return QVariant();
		} else {
			loco::Factory* lf = qobject_cast< loco::Factory* >( pl.instance() );
			if( lf == 0 ) {
			    Error( "Wrong plugin type" );
                webFrame_->evaluateJavaScript( errCBack_ );    
			}
			factories.push_back( lf );
			lf->SetContext( this );
			const QString in = GetNextInstanceName();
			webFrame_->addToJavascriptWindowObject( in, lf );
			return webFrame_->evaluateJavaScript( in ); 
		}	 
	}
	void addFilter( const QString& id, const QString& uri ) {
	    QPluginLoader pl( uri );
		if( !pl.load() ) {
			Error( pl.errorString() );
            webFrame_->evaluateJavaScript( errCBack_ );
			return QVariant();
		} else {
			loco::Filter* lf = qobject_cast< loco::Filter* >( pl.instance() );
			if( lf == 0 ) {
			    Error( "Wrong filter type" );
                webFrame_->evaluateJavaScript( errCBack_ );    
			}
			filters[ id ] = lf;
		}	  
	}
	bool hasFilter( const QString& id ) { return filters_.find( id ) != filters_.end(); }
	void addScriptFilter( const QString& js,
		                  const QString& id,
		                  const QString& jfun,
						  const QString& jerrfun = "",
						  const QString& codePlaceHolder = "",
						  const QStringList& filtersOnFilter = QStringList() ) {
	    if( !filtersOnFilter.empty() ) {
		    bool ok = true;
            QString c = js;
			for( QStringList::iterator i = filters.begin(); i != filters.end() ) {
			    c = i->Filter( c );
				if( i->Error() ) {
					error( i->getLastError() );
					ok = false;
					break;
				}
			}
			if( !ok ) return false;
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



/*
var factory = loco_context__.factory( "plugin URI" ); // "" == default factory
factory.create( "WebView", <json data> );
-----------------------


QVariant createJScriptObjInstance( const QString& name, const QVariantMap& params ) {
    QObject* obj = factory->create( name, params );
	if( !obj ) {
	    error( "..." )
		webFrame->evaluateJavaScript( errCBack );
		return QVariant();
	}
	obj->setParent( this );
	const QString in = GetNextInstanceName();
    jscriptInstance.push_back( std::make_pair( in, obj ) ) );
    webFrame->addToJavascriptWindowObject( in, obj );
	return webFrame->evaluateJavaScript( in );

QString read( const QString& url ) {
	    if( redirectCnt_ > 1 ) return "";
		QNetworkAccessManager *networkMgr = new QNetworkAccessManager(this);
        QNetworkReply *reply = networkMgr->get( QNetworkRequest( QUrl( url ) ) );
 
        QEventLoop loop;
        QObject::connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
 
        // Execute the event loop here, now we will wait here until readyRead() signal is emitted
        // which in turn will trigger event loop quit.
        loop.exec();
        QVariant possibleRedirectUrl =  reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
		QUrl urlRedirectedTo = redirectUrl(possibleRedirectUrl.toUrl(), urlRedirectedTo);
		
		// If the URL is not empty, we're being redirected.
		if(!urlRedirectedTo.isEmpty()) {
			++redirectCnt_;
			std::cout << urlRedirectedTo.toString().toStdString() << std::endl;
			return read( urlRedirectedTo.toString() );
		}
		else { redirectCnt_ = 0; return reply->readAll(); }
	}
QUrl redirectUrl(const QUrl& possibleRedirectUrl, const QUrl& oldRedirectUrl) const {
        QUrl redirectUrl;
        if(!possibleRedirectUrl.isEmpty() && possibleRedirectUrl != oldRedirectUrl)
            redirectUrl = possibleRedirectUrl;
        return redirectUrl;
    }

*/
