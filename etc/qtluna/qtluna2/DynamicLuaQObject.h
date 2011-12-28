#pragma once

#include <stdexcept>

#include "DynamicQObject.h"

#include <QMetaType>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

typedef QList< QMetaType::Type > ParameterTypes;


void PushLuaValue( lua_State* L, QMetaType::Type type, void* arg ) {
	switch( type ) {
	case QMetaType::Int: lua_pushinteger( L, *reinterpret_cast< int* >( arg ) );
		                 break;
	case QMetaType::Double: lua_pushnumber( L, *reinterpret_cast< double* >( arg ) );
		                    break;
	case QMetaType::QString: lua_pushstring( L, (reinterpret_cast< QString* >( arg ))->toAscii().constData() );
		                     break;
	default: throw std::logic_error( "Only int, double and QString implemented at this time" );
	}
}

class LuaCBackSlot : public DynamicSlot {
public:
	LuaCBackSlot( lua_State* L, const ParameterTypes& p, int luaCBackRef ) 
		: L_( L ), paramTypes_( p ), luaCBackRef_( luaCBackRef ) {}
	virtual void call(QObject* /*sender*/, void **arguments ) {
		for( ParameterTypes::const_iterator i = paramTypes_.begin();
			 i != paramTypes_.end(); ++i, ++arguments ) {
		    PushLuaValue( L_, *i, *arguments );
		}
		lua_rawgeti( L_, LUA_REGISTRYINDEX, luaCBackRef_ );
		lua_pcall( L_, paramTypes_.size(), 0, 0 );
	}
private:
	lua_State* L_;
	ParameterTypes paramTypes_;
	int luaCBackRef_;
};


struct SlotInfo {
    int luaCBackRef;
	ParameterTypes paramTypes;
	SlotInfo() : luaCBackRef( 0 ) {}
	SlotInfo( int lref, const ParameterTypes& p ) : luaCBackRef( lref ), paramTypes( p ) {}
};
typedef QMap< QString, SlotInfo > SlotInfoMap;
class DynamicLuaQObject: public DynamicQObject
{
public:
	DynamicLuaQObject() : L_( 0 ) {}
	DynamicLuaQObject( lua_State* L ) : L_( L ) {}
	void SetLuaState( lua_State* L ) { L_ = L; };
	void RegisterSlot( const QString& slot, const ParameterTypes& paramTypes, int luaCBackRef ) {
	    luaRefMap_[ slot ] = SlotInfo( luaCBackRef, paramTypes );
	}
	virtual DynamicSlot *createSlot( char *slot ) {
		return new LuaCBackSlot( L_, luaRefMap_[ slot ].paramTypes, luaRefMap_[ slot ].luaCBackRef );
	}
private:
	lua_State* L_;
	SlotInfoMap luaRefMap_;
};
