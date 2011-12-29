#include "LuaContext.h"

#include "MyObject.h"

//------------------------------------------------------------------------------
int main() {
	try {
		qlua::LuaContext ctx;
        MyObject myobj;
		//only add a single method to the Lua table
        ctx.AddQObject( &myobj, "myobj", QStringList() << "emitSignal" );
	    ctx.Eval( "qtconnect( myobj, 'aSignal()', function() print( 'Lua callback called!' ); end )" );
		//both the following lines work
		//ctx.Eval( "qtconnect( myobj, 'aSignal()', myobj.qobject__, 'aSlot()' )" );
		ctx.Eval( "qtconnect( myobj, 'aSignal()', myobj, 'aSlot()' )" );
        ctx.Eval( "myobj.emitSignal()" );
	} catch( const std::exception& e ) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}