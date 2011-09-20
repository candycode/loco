#pragma once
//#SRCHEADER
#include <QString>
#include <QVariant>
#include <QScopedPointer>
#include <QtScript/QScriptEngine>

#include "LocoIJSInterpreter.h"


///@warning Scripts (e.g. coffeescript) referencing the global 'this' object won't work with QtScript
///To make it work simply remove references to the global 'this' object

namespace loco {

class DefaultJS : public IJSInterpreter {
	Q_OBJECT
public:
	QVariant EvaluateJavaScript( const QString& code ) {
		return jsEngine_.evaluate( code ).toVariant();
	}
	void AddObjectToJS( const QString& name, QObject* obj ) {
		jsEngine_.globalObject().setProperty( name, jsEngine_.newQObject( obj, QScriptEngine::AutoOwnership ) );
	}
	void AddObjectToJS( const QString& name, QObject* obj, QScriptEngine::ValueOwnership vo ) {
		jsEngine_.globalObject().setProperty( name, jsEngine_.newQObject( obj, vo ) );
	}
	void Init() { 
		emit JavaScriptContextCleared(); 
	}
signals:
	void JavaScriptContextCleared();
private slots:
	void EmitJavaScriptContextCleared() { emit JavaScriptContextCleared(); }
private:
    QScriptEngine jsEngine_;
};

}
