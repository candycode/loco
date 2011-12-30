#pragma once

#include <QMap>
#include <QList>
#include <QMetaType>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

namespace qlua {

class LuaContext;

typedef QList< QMetaType::Type > ParameterTypes;

void PushLuaValue( LuaContext* lc, QMetaType::Type type, void* arg );

//------------------------------------------------------------------------------
class LuaCBackMethod {
public:
	LuaCBackMethod( LuaContext* lc, const ParameterTypes& p, int luaCBackRef ) 
		: lc_( lc ), paramTypes_( p ), luaCBackRef_( luaCBackRef ) {}
	void Invoke( void **arguments );
private:
	LuaContext* lc_;
	ParameterTypes paramTypes_;
	int luaCBackRef_;
};


typedef int LuaCBackRef;
typedef int MethodId;

//------------------------------------------------------------------------------
class LuaCallbackDispatcher : public QObject {
public:
	LuaCallbackDispatcher( QObject* parent = 0 ) : QObject( parent ), lc_( 0 ) {}
	LuaCallbackDispatcher( LuaContext* lc ) : lc_( lc ) {}
	int qt_metacall( QMetaObject::Call c, int id, void **arguments ); 
    bool Connect( QObject *obj, 
		          int signalIdx,
				  const QList< QMetaType::Type >& paramTypes,
				  int luaCBackRef );
	void SetLuaContext( LuaContext* lc ) { lc_ = lc; };
	virtual ~LuaCallbackDispatcher() {
		for( QList< LuaCBackMethod* >::iterator i = luaCBackMethods_.begin();
			 i != luaCBackMethods_.end(); ++i ) {
            delete *i;
		}
	}
private:
	LuaContext* lc_;
	QList< LuaCBackMethod* > luaCBackMethods_;
	QMap< LuaCBackRef, MethodId > cbackToMethodIndex_;
   
};
}