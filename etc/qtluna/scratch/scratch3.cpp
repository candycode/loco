//Who:  Ugo Varetto
//What: Shows how to traverse a lua table in C
//How:  Use 'lua_next' to iterate over <value,key> pairs;
//      'lua_next' replaces the key on top of the stack with
//      the next <value,key> pair
//      1) push nil on stack
//      2) invoke 'next' with index of table until it does not return 0,
//         pop value from stack
//      3) pop nil from stack
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <iostream>
#include <string>


#include <QString>
#include <QVariant>
#include <QVarianMap>


QString LuaKeyToQString( lua_State* L, int idx ) {
	if( lua_isstring( L, idx ) ) {
	    return lua_tostring( L, idx );
	} else if( lua_isnumber( L, idx ) {
		return QString("%1").arg( lua_tointeger( L, idx ) );
	} else return "";
}

template < typename ToT, typename FromT >
bool ConvertibleTo( FromT v ) {
    return FromT( ToT( v ) ) == v;
}

QVariant LuaValueToQVariant( lua_State* L, int idx ) {
	if( lua_isstring( L, idx ) ) {
	    return lua_tostring( L, idx );
	} else if( lua_isnumber( L, idx ) {
		const double N = lua_tonumber( L, idx );
		if( ConvertibleTo< char >( N ) ) return char( N );
		else if( ConvertibleTo< unsigned char >( N ) ) return (unsigned char)( N );
		else if( ConvertibleTo< short >( N ) ) return short( N );
		else if( ConvertibleTo< unsigned short >( N ) ) return (unsigned short)( N );
		else if( ConvertibleTo< int >( N ) ) return int( N );
		else if( ConvertibleTo< unsigned int >( N ) ) return (unsigned int)( N );
		else if( ConvertibleTo< long >( N ) ) return long( N );
		else if( ConvertibleTo< unsigned long >( N ) ) return (unsigned long)( N );
		else if( ConvertibleTo< long long >( N ) ) return long( N );
		else if( ConvertibleTo< unsigned long long >( N ) ) return (unsigned long long)( N );
		else if( ConvertibleTo< float >( N ) ) return float( N );
		else return N;
	} else if( lua_isboolean( L, idx ) {
		return lua_toboolean( L, idx );
	} else if( lua_islightuserdata( L, idx ) ) {
		return lua_topointer( L, idx ); 
	} else return "";
}


QVarianMap ParseLuaTable( lua_State* L ) {
    QVariantMap m;
    lua_pushnil(L);  // first key
	const int stackTableIndex = -2;
    while( lua_next( L, stackTableIndex ) != 0 ) {
		/* uses 'key' (at index -2) and 'value' (at index -1) */
		QString key = LuaKeyToQString( L, -2 );
		QVariant value = lua_istable( L, -1 ) ? ParseLuaTable( L ) : 
			             LuaValueToQVariant( L, -1 );
		m[ key ] = value;
		lua_pop(L, 1);
    }
	lua_pop( L, 1 ); // remvove nil
	return m;
}


//------------------------------------------------------------------------------
int MyCFunction( lua_State* L ) {
	std::cout << "MyCFunction Called" << std::endl; 
	if( !lua_istable( L, -1 ) ) {
		std::cerr << "Not a table" << std::endl;
		return 0;
	}
    PrintVariant( ParseLuaTable( L ) );
	lua_pop( L, 1 ); // remvove nil
	return 0;
}

//------------------------------------------------------------------------------
void ReportErrors( lua_State* L, int status ) {
	if( status != 0 ) {
		std::string err = lua_tostring( L, -1 );
		lua_pop( L, 1 );
		std::cerr << err << std::endl;
	}
}	

//------------------------------------------------------------------------------
int main( int argc, char** argv ) {
	lua_State* L = luaL_newstate();
    luaL_openlibs( L );
   
    lua_pushcfunction( L, &MyCFunction );
    lua_setglobal( L, "myCFunction" );

	ReportErrors( L, luaL_dostring( L, "myCFunction( {1,2,3,table={key='value'} } )" ) ); 

    lua_close( L );
    return 0;	
}
