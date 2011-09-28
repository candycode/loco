#pragma once
//#SRCHEADER
#include <QString>
#include <QVariant>
#include <QPointer>
#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebFrame>

#include "LocoIJSInterpreter.h"

namespace loco {

class WebKitJSCoreWrapper : public IJSInterpreter {
	Q_OBJECT
public:
	WebKitJSCoreWrapper( QWebPage* wp = 0 ) : wp_( wp ), wf_( 0 ) {
	    if( wp ) wf_ = wp->mainFrame();
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
	void SetWebPage( QWebPage* wp ) { 
		wp_ = wp;
		wf_ = wp_->mainFrame();
		connect( wf_, SIGNAL( javaScriptWindowObjectCleared() ),
			     this, SIGNAL( JavaScriptContextCleared() ) );
	}
signals:
	void JavaScriptContextCleared(); // = javaScriptWindowObjectCleared()
private:
    QPointer< QWebPage > wp_;
    QPointer< QWebFrame > wf_;
};

}
