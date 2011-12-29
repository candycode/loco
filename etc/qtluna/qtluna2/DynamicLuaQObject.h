#pragma once

#include <stdexcept>

#include <QHash>
#include <QMetaType>

#include "DynamicQObject.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

namespace qlua {

class LuaContext;

typedef QList< QMetaType::Type > ParameterTypes;

void PushLuaValue( LuaContext* lc, QMetaType::Type type, void* arg );

class LuaCBackSlot : public DynamicSlot {
public:
	LuaCBackSlot( LuaContext* lc, const ParameterTypes& p, int luaCBackRef ) 
		: lc_( lc ), paramTypes_( p ), luaCBackRef_( luaCBackRef ) {}
	void Invoke( void **arguments );
private:
	LuaContext* lc_;
	ParameterTypes paramTypes_;
	int luaCBackRef_;
};

struct SlotInfo {
    int luaCBackRef;
	ParameterTypes paramTypes;
	SlotInfo() : luaCBackRef( 0 ) {}
	SlotInfo( int lref, const ParameterTypes& p ) : luaCBackRef( lref ), paramTypes( p ) {}
};
typedef QHash< QString, SlotInfo > SlotInfoMap;
class DynamicLuaQObject: public DynamicQObject
{
public:
	DynamicLuaQObject() : lc_( 0 ) {}
	DynamicLuaQObject( LuaContext* lc ) : lc_( lc ) {}
	void SetLuaContext( LuaContext* lc ) { lc_ = lc; };
	void RegisterSlot( const QString& slot, const ParameterTypes& paramTypes, int luaCBackRef ) {
	    luaRefMap_[ slot ] = SlotInfo( luaCBackRef, paramTypes );
	}
	DynamicSlot *createSlot( const char *slot );
private:
	LuaContext* lc_;
	SlotInfoMap luaRefMap_;
};
}