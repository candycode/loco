#pragma once
//#srcheader
#include <QtWebKit/QWebView>
#include <String>
#include <QMap>
#include <QFile>
#include <QtWebKit/QwebFrame>
#include "LocoIWebConfig.h"
#include <iostream>
#include <QApplication>
namespace loco {

typedef QWebView* QWebViewPtr;
typedef IWebConfig* IWebConfigPtr;
class WebView : public QObject {
	Q_OBJECT
public: WebView( QApplication* app, QWebViewPtr wv = 0 ) : app_( app ), wv_( wv ) { 
			if( wv_ ) connect( wv_, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)) );
			/*printf("WebView::WebView\n");*/}
	void AddConfig( const QString& name, IWebConfigPtr cfg ) {
		configMap_[ name ] = cfg ;
	}
	void Set( QWebViewPtr wv ) { wv_ = wv; }
public slots:
	void setHtml( const QString& code, const QString& configId ) {
		bool found = false;
		if( configMap_.find( configId ) != configMap_.end() ) found = true;
		if( found ) configMap_[ configId ]->Shutdown( wv_ );
		wv_->setHtml( code );
		if( found ) configMap_[ configId ]->Init( wv_ );
    }
	void destroy() { delete this; }
	void loadHtml( const QString& url, const QString& configId, const QString& cback ) {
		//std::cout << url << std::endl;
		
		bool found = false;
		if( configMap_.find( configId ) != configMap_.end() ) found = true;
		if( found ) configMap_[ configId ]->Shutdown( wv_ );
		configIdTmp_ = configId;
		cbackTmp_ = cback;
		wv_->load( url );
	}
	QString readTxt( const QString& path ) {
		QFile f(path);
		f.open( QIODevice::ReadOnly );
		return f.readAll();
	}
	int exec() { return app_->exec(); }
private slots:
	void loadFinished(bool ok) {
		if( ok ) {
			bool found = false;
			if( configMap_.find( configIdTmp_ ) != configMap_.end() ) found = true;
			if( found ) configMap_[ configIdTmp_ ]->Init( wv_ );
			wv_->page()->currentFrame()->evaluateJavaScript( cbackTmp_ );
		}
	}

private:
	QApplication* app_;
	QWebViewPtr wv_;
	typedef QMap< QString, IWebConfigPtr > ConfigMap;
	ConfigMap configMap_;
	mutable QString configIdTmp_;
	mutable QString cbackTmp_;
};
}