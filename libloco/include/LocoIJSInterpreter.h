#pragma once
//#SRCHEADER
#include <QObject>
#include <QString>
#include <QVariant>
#include <QtScript/QScriptEngine>

namespace loco {

//derivation from QObject required to store pointers into QPointers

class IJSInterpreter : public QObject {
    Q_OBJECT
public:
	virtual QVariant EvaluateJavaScript( const QString& code ) = 0;
	virtual void AddObjectToJS( const QString& name, QObject* obj, QScriptEngine::ValueOwnership vo ) = 0;
	virtual void AddObjectToJS( const QString& name, QObject* obj ) = 0;
	virtual void Init() = 0;
	virtual QString Name() const = 0; //used to pass information on type and version e.g. QtScript vs Webkit JS core
	virtual bool SetAllowInterrupt( bool  ) = 0; //returns false if not supported
	virtual	bool GetAllowInterrupt() const = 0; //always return false if not supported
	virtual ~IJSInterpreter() {}
signals:
	/*virtual*/ void JavaScriptConsoleMessage( const QString& /*text*/, int /*line*/, const QString& /*source id*/ );
};

}
