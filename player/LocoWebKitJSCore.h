#pragma once
//#SRCHEADER
#include <QString>
#include <QVariant>
#include <QPointer>
#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebFrame>

#include "LocoIJSInterpreter.h"

namespace loco {

class WebKitJSCore : public IJSInterpreter {
	Q_OBJECT
public:
	WebKitJSCore() : wp_( new QWebPage )  {
		wf_ = wp_->mainFrame();
		connect( wf_, SIGNAL( javaScriptWindowObjectCleared() ),
			     this, SIGNAL( JavaScriptContextCleared() ) );
        wp_->setParent( this );
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
    ~WebKitJSCore() {
        QObject* p = parent();
    }
signals:
	void JavaScriptContextCleared(); // = javaScriptWindowObjectCleared()
private:
    QWebPage* wp_;
    QPointer< QWebFrame > wf_;
};

}
