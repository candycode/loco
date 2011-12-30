#include <cassert>
#include <QPointer>
#include <iostream>
#include "LuaContext.h"

#include "MyObject.h"

//------------------------------------------------------------------------------
int main() {
	try {
		qlua::LuaContext ctx;
        
		MyObject myobj;
		myobj.setObjectName( "MyObject" );
		//only add a single method to the Lua table
		ctx.AddQObject( &myobj, "myobj", qlua::LuaContext::QOBJ_NO_DELETE, QStringList() << "emitSignal" );
	    ctx.Eval( "qtconnect( myobj, 'aSignal()', function() print( 'Lua callback called!' ); end )" );
		ctx.Eval( "print( 'object name: ' .. myobj.objectName )" );
		//both the following lines work
		//ctx.Eval( "qtconnect( myobj, 'aSignal()', myobj.qobject__, 'aSlot()' )" );
		ctx.Eval( "qtconnect( myobj, 'aSignal()', myobj, 'aSlot()' )" );
        ctx.Eval( "myobj.emitSignal()" );

		MyObject* myobj2 = new MyObject;
		QPointer< MyObject > pMyObject = myobj2;
		myobj2->setObjectName( "MyObject2" );
		ctx.AddQObject( myobj2, "myobj2", qlua::LuaContext::QOBJ_IMMEDIATE_DELETE );
		ctx.Eval( "print( 'object 2 name: '..myobj2.objectName )" );
		ctx.Eval( "getmetatable(myobj2).__gc()" );
		assert( pMyObject.isNull() );

	} catch( const std::exception& e ) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}