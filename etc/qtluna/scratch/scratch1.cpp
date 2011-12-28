//Who:  Ugo Varetto
//What: Shows how to store C closures in tables
//Why:  useful when invoking the same method-invocation function with different hidden parameters
//      like the index of a function/method in a table or index of Qt signal/slot with the 
//How:  1) create table and push method name
//      2) push closure-specific data
//      3) invoke 'pushcclosure' with the number of parameters to store in closure context
//      4) invoke standard 'rawset' to store function 
//Also: another technique is to:
//      1) [C] store (no more)"hidden" paramter in the table
//      2) [Lua] invoke the function from Lua with the ':' operator which causes the same
//         table to be passed as the first parameter to the invoked function
//      3) [C] extract the required parameters from the table

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <iostream>
#include <string>

//------------------------------------------------------------------------------
class MyClass {
public:
    static int InvokeMethod( lua_State* L ) {
    	MyClass* obj = reinterpret_cast< MyClass* >( lua_touserdata( L, lua_upvalueindex( 1 ) ) );
    	return obj->PrivateMethod( L );
    }
private:
    int PrivateMethod( lua_State* L ) {
		if( lua_gettop( L ) < 2 ) {
		    lua_pushstring( L, "Two arguments required!" );
		    lua_error( L );
			return 0;
		}
		const char* p1 = lua_tostring( L, 1 );
		const char* p2 = lua_tostring( L, 2 );
		std::cout << "Invoked from Lua; parameters: " << p1 << ' ' << p2 << std::endl;
		return 0;
    }    
};

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
    
    MyClass myObject;
    lua_newtable( L ); 
    lua_pushstring( L, "method" );
    lua_pushlightuserdata( L, &myObject );
    lua_pushcclosure( L, &MyClass::InvokeMethod, 1 );
    lua_rawset( L, -3 );
    lua_setglobal( L, "mytable" );

	ReportErrors( L, luaL_dostring( L, "mytable.method('first','second')" ) ); 

    lua_close( L );
    return 0;	
}
