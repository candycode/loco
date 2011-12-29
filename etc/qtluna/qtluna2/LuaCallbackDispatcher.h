#pragma once

#include <stdexcept>

#include <QHash>
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
class LuaCBackSlot {
public:
	LuaCBackSlot( LuaContext* lc, const ParameterTypes& p, int luaCBackRef ) 
		: lc_( lc ), paramTypes_( p ), luaCBackRef_( luaCBackRef ) {}
	void Invoke( void **arguments );
private:
	LuaContext* lc_;
	ParameterTypes paramTypes_;
	int luaCBackRef_;
};

//------------------------------------------------------------------------------
struct SlotInfo {
    int luaCBackRef;
	ParameterTypes paramTypes;
	SlotInfo() : luaCBackRef( 0 ) {}
	SlotInfo( int lref, const ParameterTypes& p ) : luaCBackRef( lref ), paramTypes( p ) {}
};
typedef QHash< QString, SlotInfo > SlotInfoMap;

//------------------------------------------------------------------------------
class LuaCallbackDispatcher : public QObject {
public:
	LuaCallbackDispatcher( QObject* parent = 0 ) : QObject( parent ), lc_( 0 ) {}
	LuaCallbackDispatcher( LuaContext* lc ) : lc_( lc ) {}
	int qt_metacall( QMetaObject::Call c, int id, void **arguments ); 
    bool Connect( QObject *obj, const char *signal, const char *slot );
	void SetLuaContext( LuaContext* lc ) { lc_ = lc; };
	void RegisterSlot( const QString& slot, const ParameterTypes& paramTypes, int luaCBackRef ) {
	    luaRefMap_[ QMetaObject::normalizedSignature( slot.toAscii().constData() ) ] = SlotInfo( luaCBackRef, paramTypes );
	}
	virtual ~LuaCallbackDispatcher() {
		for( QList< LuaCBackSlot* >::iterator i = luaCBackSlots_.begin();
			 i != luaCBackSlots_.end(); ++i ) {
            delete *i;
		}
	}
private:
	LuaContext* lc_;
	SlotInfoMap luaRefMap_;
	QList< LuaCBackSlot* > luaCBackSlots_;
	QHash< QByteArray, int > slotIndexMap_;
   
};
}