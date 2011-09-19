#pragma once
//#SRCHEADER
#include <QString>
#include <QVariant>
#include <QScopedPointer>
#include <QtWebkit/QWebPage>
#include <QtWebkit/QWebFrame>

#include "LocoIJSInterpreter.h"

namespace loco {

class WebKitJSCore : public IJSInterpreter {
	Q_OBJECT
public:
	WebKitJSCore()  {
		wf_ = wp_.mainFrame();
		connect( wf_, SIGNAL( javaScriptWindowObjectCleared() ),
			     this, SLOT( EmitJavaScriptContextCleared() ) );
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
signals:
	void JavaScriptContextCleared(); // = javaScriptWindowObjectCleared()
private slots:
	void EmitJavaScriptContextCleared() { emit JavaScriptContextCleared(); }
private:
    QWebPage wp_;
    QWebFrame* wf_;
};

}