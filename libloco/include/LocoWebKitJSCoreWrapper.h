#pragma once
//#SRCHEADER
#include <QObject>
#include <QString>
#include <QVariant>
#include <QPointer>
#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebFrame>
#include "LocoWebPage.h"

#include "LocoIJSInterpreter.h"

namespace loco {

class WebKitJSCoreWrapper : public IJSInterpreter {
	Q_OBJECT
public:
	WebKitJSCoreWrapper( QWebPage* wp = 0 ) : wp_( wp ), wf_( 0 ) {
	    if( wp ) SetWebPage( wp );
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
	QString Name() const { return "webkit-js-core-wrapper"; }
	void SetWebPage( QWebPage* wp ) { 
		wp_ = wp;
		wf_ = wp_->mainFrame();
		connect( wf_,  SIGNAL( javaScriptWindowObjectCleared() ),
			     this, SIGNAL( JavaScriptContextCleared() ) );
	}
	bool SetAllowInterrupt( bool yes ) {
		qobject_cast< WebPage* >( wp_ )->SetAllowInterruptJavaScript( yes );
		return true;
	}
	bool GetAllowInterrupt() const {
		return qobject_cast< WebPage* >( wp_ )->GetAllowInterruptJavaScript();
	}
signals:
	void JavaScriptContextCleared(); // = javaScriptWindowObjectCleared()
private:
    QPointer< QWebPage > wp_;
    QPointer< QWebFrame > wf_;
};

}
