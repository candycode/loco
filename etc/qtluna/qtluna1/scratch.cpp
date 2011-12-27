extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <iostream>


class MyClass {
public:
    static int InvokeMethod( lua_State* L ) {
    	MyClass* obj = reinterpret_cast< MyClass* >( lua_tolightuserdata( L ) );
    	obj->PrivateMethod();
    }
private:
    void PrivateMethod() {
    	std::cout << "Invoked from Lua" << std::endl;
    }    
};



int main( int argc, char** argv ) {
	lua_State* L = luaL_newstate();
    luaL_openlibs( L );
    
    MyClass myObject;
    lua_newtable( L ); 
    lua_pushstring( L, "method" );
    lua_pushlightuserdata( &myObject );
    lua_pushcclosure( L, &MyClass::InvokeMethod, 1 );

    lua_setglobal( L, "mytable" );

    luaL_dostring( "mytable.method()" );

    lua_close( L );
    return 0;	
}

