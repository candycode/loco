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

//------------------------------------------------------------------------------
int MyCFunction( lua_State* L ) {
	std::cout << "MyCFunction Called" << std::endl; 
	if( !lua_istable( L, -1 ) ) {
		std::cerr << "Not a table" << std::endl;
		return 0;
	}
	std::cout << "Stack size: " << lua_gettop( L ) << std::endl;
	// every successful invocation of lua_next replaces
	// key on stack with next <value,key> pair;
	// a key must always be present on stack for correct
	// lua_next operation that is why only the value is
	// removed from the stack
    lua_pushnil(L);  // first key
	std::cout << "Stack size[ lua_pushnil ]: " << lua_gettop( L ) << std::endl;
	const int stackTableIndex = -2;
    while( lua_next( L, stackTableIndex ) != 0 ) {
		std::cout << "Stack size[ lua_next ]: " << lua_gettop( L ) << std::endl;
		/* uses 'key' (at index -2) and 'value' (at index -1) */
		printf("%s - %s\n",
			  lua_typename(L, lua_type(L, -2)),
			  lua_typename(L, lua_type(L, -1)));
		/* removes 'value'; keeps 'key' for next iteration */
		lua_pop(L, 1);
    }
	std::cout << "Stack size: " << lua_gettop( L ) << std::endl;
	lua_pop( L, 1 ); // remvove nil
	std::cout << "Stack size: " << lua_gettop( L ) << std::endl;
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
