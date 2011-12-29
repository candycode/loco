#pragma once
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <iostream>
#include <string>

#include <QString>
#include <QVariant>
#include <QVariantMap>
#include <QGenericArgument>

namespace qlua {

//------------------------------------------------------------------------------
inline
QString LuaKeyToQString( lua_State* L, int idx ) {
	if( lua_isnumber( L, idx ) ) {
		return QString( "%1" ).arg( lua_tointeger( L, idx ) );
	} else if( lua_isstring( L, idx ) ) {
	    return lua_tostring( L, idx );
	} else return "";
}

//------------------------------------------------------------------------------
template < typename ToT, typename FromT >
bool ConvertibleTo( FromT v ) {
    return FromT( ToT( v ) ) == v;
}

//------------------------------------------------------------------------------
inline
QVariant LuaValueToQVariant( lua_State* L, int idx ) {
	if( lua_isboolean( L, idx ) ) {
		return bool( lua_toboolean( L, idx ) );
	} else if( lua_isnumber( L, idx ) ) {	
#ifdef QLUA_CONVERT_NUMBER
		const double N = lua_tonumber( L, idx );
		if( ConvertibleTo< int >( N ) ) return int( N );
		else if( ConvertibleTo< unsigned int >( N ) ) return (unsigned int)( N );
		else if( ConvertibleTo< long long >( N ) ) return long( N );
		else if( ConvertibleTo< unsigned long long >( N ) ) return (unsigned long long)( N );
		else if( ConvertibleTo< float >( N ) ) return float( N );
		else return N;
#else
		return lua_tonumber( L, idx );
#endif
	} else if( lua_islightuserdata( L, idx ) ) {
		return lua_topointer( L, idx ); 
	} else if( lua_isstring( L, idx ) ) {
	    return lua_tostring( L, idx );
	} else return QVariant();
}

//------------------------------------------------------------------------------
inline
QVariantMap ParseLuaTable( lua_State* L, int stackTableIndex, bool removeTable = true ) {
    QVariantMap m;
    lua_pushnil(L);  // first key
	stackTableIndex = stackTableIndex < 0 ? stackTableIndex - 1 : stackTableIndex;
    while( lua_next( L, stackTableIndex ) != 0 ) {
		/* uses 'key' (at index -2) and 'value' (at index -1) */
		QString key = LuaKeyToQString( L, -2 );
		QVariant value = lua_istable( L, -1 ) ? ParseLuaTable( L, -1, false ) : 
			             LuaValueToQVariant( L, -1 );
		m[ key ] = value;
		lua_pop(L, 1);
    }
	if( removeTable ) lua_pop( L, 1 ); // remvove table
	return m;
}
inline
QVariantList ParseLuaTableAsVariantList( lua_State* L, int stackTableIndex, bool removeTable = true ) {
    QVariantList l;
    lua_pushnil(L);  // first key
	stackTableIndex = stackTableIndex < 0 ? stackTableIndex - 1 : stackTableIndex;
    while( lua_next( L, stackTableIndex ) != 0 ) {
		/* uses 'key' (at index -2) and 'value' (at index -1) */
		QVariant value = lua_istable( L, -1 ) ? ParseLuaTable( L, -1, false ) : 
			             LuaValueToQVariant( L, -1 );
		l.push_back( value );
		lua_pop(L, 1);
    }
	if( removeTable ) lua_pop( L, 1 ); // remvove table
	return l;
}

template < typename T >
QList< T > ParseLuaTableAsNumberList( lua_State* L, int stackTableIndex, bool removeTable = true ) {
    QList< T > l;
    lua_pushnil(L);  // first key
	stackTableIndex = stackTableIndex < 0 ? stackTableIndex - 1 : stackTableIndex;
    while( lua_next( L, stackTableIndex ) != 0 ) {
		/* uses 'key' (at index -2) and 'value' (at index -1) */
	    l.push_back( T( luaL_checknumber( L, -1 ) ) );
		lua_pop(L, 1);
    }
	if( removeTable ) lua_pop( L, 1 ); // remvove table
	return l;
}

inline
QStringList ParseLuaTableAsStringList( lua_State* L, int stackTableIndex, bool removeTable = true ) {
    QStringList l;
    lua_pushnil(L);  // first key
	stackTableIndex = stackTableIndex < 0 ? stackTableIndex - 1 : stackTableIndex;
    while( lua_next( L, stackTableIndex ) != 0 ) {
		/* uses 'key' (at index -2) and 'value' (at index -1) */
		l.push_back( luaL_checkstring( L, -1 ) );
		lua_pop(L, 1);
    }
	if( removeTable ) lua_pop( L, 1 ); // remvove table
	return l;
}

//------------------------------------------------------------------------------
inline
void VariantMapToLuaTable( const QVariantMap& vm, lua_State* L ) {
    lua_newtable( L ); 
	for( QVariantMap::const_iterator i = vm.begin(); i != vm.end(); ++i ) {
		lua_pushstring( L, i.key().toAscii().constData() );
		switch( i.value().type() ) {
			case QVariant::Map: VariantMapToLuaTable( i.value().toMap(), L );
			                    break;
			case QVariant::String: lua_pushstring( L, i.value().toString().toAscii().constData() );
				                   break; 
			case QVariant::Int: lua_pushinteger( L, i.value().toInt() );
	                            break;
            case QVariant::UInt: lua_pushnumber( L, i.value().toUInt() );
	                             break;
            case QVariant::LongLong: lua_pushnumber( L, i.value().toLongLong() );
	                                 break;
            case QVariant::ULongLong: lua_pushnumber( L, i.value().toULongLong() );
	                                  break;
			case QVariant::Bool: lua_pushboolean( L, i.value().toBool() );
	                             break;
			default: break;
		}
		lua_rawset( L, -3 );
	}
}

inline
void VariantListToLuaTable( const QVariantList& vl, lua_State* L ) {
    lua_newtable( L ); 
	int i = 1;
	for( QVariantList::const_iterator v = vl.begin(); v != vl.end(); ++v, ++i ) {
		lua_pushinteger( L, i );
		switch( v->type() ) {
			case QVariant::Map: VariantMapToLuaTable( v->toMap(), L );
			                    break;
            case QVariant::List: VariantListToLuaTable( v->toList(), L );
			                    break;
			case QVariant::String: lua_pushstring( L, v->toString().toAscii().constData() );
				                   break; 
			case QVariant::Int: lua_pushinteger( L, v->toInt() );
	                            break;
            case QVariant::UInt: lua_pushnumber( L, v->toUInt() );
	                             break;
            case QVariant::LongLong: lua_pushnumber( L, v->toLongLong() );
	                                 break;
            case QVariant::ULongLong: lua_pushnumber( L, v->toULongLong() );
	                                  break;
			case QVariant::Bool: lua_pushboolean( L, v->toBool() );
	                             break;
			case QVariant::Double: lua_pushnumber( L, v->toDouble() );
	                             break;
			default: break;
		}
		lua_rawset( L, -3 );
	}
}

template < typename T >
void ListToLuaTable( const QList< T >& vl, lua_State* L ) {
    lua_newtable( L ); 
	int i = 1;
	for( QList< T >::const_iterator v = vl.begin(); v != vl.end(); ++v, ++i ) {
		lua_pushinteger( L, i );
	    lua_pushnumber( L, *v );
		lua_rawset( L, -3 );
	}
}

inline
void StringListToLuaTable( const QStringList& vl, lua_State* L ) {
    lua_newtable( L ); 
	int i = 1;
	for( QStringList::const_iterator v = vl.begin(); v != vl.end(); ++v, ++i ) {
		lua_pushinteger( L, i );
	    lua_pushstring( L, v->toAscii().constData() );
		lua_rawset( L, -3 );
	}
}
}