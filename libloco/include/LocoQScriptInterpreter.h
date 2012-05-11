#pragma once
////////////////////////////////////////////////////////////////////////////////
//Copyright (c) 2012, Ugo Varetto
//All rights reserved.
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions are met:
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder nor the
//      names of its contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//DISCLAIMED. IN NO EVENT SHALL UGO VARETTO BE LIABLE FOR ANY
//DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////

#include <QString>
#include <QVariant>
#include <QScopedPointer>
#include <QStringList>
#include <QtScript/QScriptEngine>

#include "LocoIJSInterpreter.h"


///@warning Scripts (e.g. coffeescript) referencing the global 'this' object won't work with QtScript
///To make it work simply remove references to the global 'this' object

namespace loco {

class QScriptInterpreter : public IJSInterpreter {
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
	bool SetAllowInterrupt( bool ) { return false; }
	bool GetAllowInterrupt() const { return false; }
signals:
	void JavaScriptContextCleared();
    void JavaScriptConsoleMessage( const QString& /*text*/, int /*line*/, const QString& /*source id*/ );
private slots:
	void EmitJavaScriptContextCleared() { emit JavaScriptContextCleared(); }
private:
    QScriptEngine jsEngine_;
};

}
