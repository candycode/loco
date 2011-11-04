#pragma once
//#SRCHEADER
#include <QString>
#include <QVariant>
#include <QScopedPointer>
#include <QStringList>
#include <QtScript/QScriptEngine>

#include "LocoIJSInterpreter.h"


///@warning Scripts (e.g. coffeescript) referencing the global 'this' object won't work with QtScript
///To make it work simply remove references to the global 'this' object

namespace loco {

class DefaultJS : public IJSInterpreter {
	Q_OBJECT
public:
	QVariant EvaluateJavaScript( const QString& code ) {
		QVariant ret = jsEngine_.evaluate( code ).toVariant();
		if( jsEngine_.hasUncaughtException() ) {
			const QString msg = jsEngine_.uncaughtException().toString();
			const int lineno = jsEngine_.uncaughtExceptionLineNumber();
            const QStringList bt = jsEngine_.uncaughtExceptionBacktrace();
            emit JavaScriptConsoleMessage( "ERROR - " + msg + "\nBack trace:\n" + bt.join( "\n" ), lineno, "" );
		}
		return ret;
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
	QString Name() const { return "qtscript"; }
signals:
	void JavaScriptContextCleared();
    void JavaScriptConsoleMessage( const QString& /*text*/, int /*line*/, const QString& /*source id*/ );
private slots:
	void EmitJavaScriptContextCleared() { emit JavaScriptContextCleared(); }
private:
    QScriptEngine jsEngine_;
};

}
