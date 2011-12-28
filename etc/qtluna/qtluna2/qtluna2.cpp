#include "LuaContext.h"

#include "MyObject.h"

//------------------------------------------------------------------------------
int main() {
	try {
	    LuaContext ctx;
        MyObject myobj;
		//only add a single method to the proxy Lua table
        ctx.AddQObject( &myobj, "myobj", QStringList() << "emitSignal" );
	    ctx.Eval( "qtconnect( myobj, 'aSignal()', function() print( 'called!' ); end )" );
        ctx.Eval( "myobj.emitSignal()" ); 
	} catch( const std::exception& e ) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}