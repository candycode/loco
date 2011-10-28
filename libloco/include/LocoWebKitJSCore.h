#pragma once
//#SRCHEADER
#include <QString>
#include <QVariant>
#include <QPointer>
#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebSettings>

#include "LocoIJSInterpreter.h"


namespace loco {

class JSCoreWebPage : public QWebPage {
    Q_OBJECT
public:
	JSCoreWebPage( QObject* parent = 0 ) : QWebPage( parent ) {}
protected:
	void javascriptConsoleMessage( const QString& t, int l, const QString& s ) {
		QWebPage::javaScriptConsoleMessage( t, l, s );
		emit JavaScriptConsoleMessage( t, l, s );
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
		QVariant v = wf_->evaluateJavaScript( code );
		wp_->extension( QWebPage::ErrorPageExtension
		return v;
	}
	void AddObjectToJS( const QString& name, QObject* obj ) {
		wf_->addToJavaScriptWindowObject( name, obj );
	}
	void AddObjectToJS( const QString& name, QObject* obj, QScriptEngine::ValueOwnership vo ) {
		wf_->addToJavaScriptWindowObject( name, obj, vo );
	}
	void Init() {}
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
