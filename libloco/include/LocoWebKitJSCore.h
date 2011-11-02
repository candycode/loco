#pragma once
//#SRCHEADER
#include <QString>
#include <QVariant>
#include <QPointer>
#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebSettings>

#include "LocoIJSInterpreter.h"

///@todo remove
#include <iostream>

namespace loco {

class JSCoreWebPage : public QWebPage {
    Q_OBJECT
public:
	JSCoreWebPage( QObject* parent = 0 ) : QWebPage( parent ) {}
protected:
	void javaScriptConsoleMessage( const QString& t, int l, const QString& s ) {
		QWebPage::javaScriptConsoleMessage( t, l, s );
		emit JavaScriptConsoleMessage( t, l, s );
	}
public:
	bool supportsExtension( Extension extension ) const { 
	    if( extension == QWebPage::ErrorPageExtension ) { 
		    return true; 
        } 
		return false;
	}
	bool extension(Extension extension, const ExtensionOption *option = 0, ExtensionReturn *output = 0) {
        if( extension != QWebPage::ErrorPageExtension ) return false;

        ErrorPageExtensionOption *errorOption = ( ErrorPageExtensionOption* ) option;
        std::cerr << "Error loading " << qPrintable(errorOption->url.toString())  << std::endl;
        if(errorOption->domain == QWebPage::QtNetwork)
            std::cerr << "Network error (" << errorOption->error << "): ";
        else if(errorOption->domain == QWebPage::Http)
            std::cerr << "HTTP error (" << errorOption->error << "): ";
        else if(errorOption->domain == QWebPage::WebKit)
            std::cerr << "WebKit error (" << errorOption->error << "): ";

        std::cerr << qPrintable(errorOption->errorString) << std::endl;

        return false;
    }

signals:
	void JavaScriptConsoleMessage( const QString&, int, const QString& );
};


class WebKitJSCore : public IJSInterpreter {
	Q_OBJECT
public:
	WebKitJSCore() : wp_( new JSCoreWebPage )  {
		wf_ = wp_->mainFrame();
		connect( wf_, SIGNAL( javaScriptWindowObjectCleared() ),
			     this, SIGNAL( JavaScriptContextCleared() ) );
		connect( wp_, SIGNAL( JavaScriptConsoleMessage( const QString&, int, const QString& ) ),
			     this, SIGNAL( JavaScriptConsoleMessage( const QString&, int, const QString& ) ) );
        wp_->setParent( this );
        wp_->settings()->setAttribute( QWebSettings::LocalContentCanAccessFileUrls, true );
        wp_->settings()->setAttribute( QWebSettings::XSSAuditingEnabled, false );
        wp_->settings()->setAttribute( QWebSettings::LocalContentCanAccessRemoteUrls, true );
		wp_->settings()->setAttribute( QWebSettings::JavascriptEnabled, true );
		wp_->settings()->setAttribute( QWebSettings::DeveloperExtrasEnabled, true );

	}
	QVariant EvaluateJavaScript( const QString& code ) {
		return wf_->evaluateJavaScript( code );
        
	}
	void AddObjectToJS( const QString& name, QObject* obj ) {
		wf_->addToJavaScriptWindowObject( name, obj );
	}
	void AddObjectToJS( const QString& name, QObject* obj, QScriptEngine::ValueOwnership vo ) {
		wf_->addToJavaScriptWindowObject( name, obj, vo );
	}
	void Init() {}
	QString Name() const { return "webkit-js-core"; }
    ~WebKitJSCore() {
        QObject* p = parent();
    }
signals:
	void JavaScriptContextCleared(); // = javaScriptWindowObjectCleared()
	void JavaScriptConsoleMessage( const QString, int, const QString& );
private:
    JSCoreWebPage* wp_;
    QPointer< QWebFrame > wf_;
};

}
