#pragma once
//#SRCHEADER
#include <QObject>
#include <QString>
#include <QVariant>
#include <QtScript/QScriptEngine>

namespace loco {

class IJSInterpreter : public QObject {
    Q_OBJECT
public:
	virtual QVariant EvaluateJavaScript( const QString& code ) = 0;
	virtual void AddObjectToJS( const QString& name, QObject* obj, QScriptEngine::ValueOwnership vo ) = 0;
	virtual void AddObjectToJS( const QString& name, QObject* obj ) = 0;
	virtual void Init() = 0;
	virtual ~IJSInterpreter() {}
};

}
