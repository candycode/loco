#include <cassert>
#include <QPointer>
#include <iostream>
#include "LuaContext.h"

#include "MyObject.h"

//------------------------------------------------------------------------------
int main() {
	try {
		qlua::LuaContext ctx;
        
		TestObject myobj;
		myobj.setObjectName( "MyObject" );
		//only add a single method to the Lua table
		ctx.AddQObject( &myobj, "myobj", qlua::LuaContext::QOBJ_NO_DELETE, QStringList() << "emitSignal" );
	    ctx.Eval( "qlua.connect( myobj, 'aSignal()', function() print( 'Lua callback called!' ); end )" );
		ctx.Eval( "print( 'object name: ' .. myobj.objectName )" );
		//both the following lines work
		//ctx.Eval( "qtconnect( myobj, 'aSignal()', myobj.qobject__, 'aSlot()' )" );
		ctx.Eval( "qlua.connect( myobj, 'aSignal()', myobj, 'aSlot()' )" );
        ctx.Eval( "myobj.emitSignal()" );

		TestObject* myobj2 = new TestObject;
		QPointer< TestObject > pMyObject = myobj2;
		myobj2->setObjectName( "MyObject2" );
		ctx.AddQObject( myobj2, "myobj2", qlua::LuaContext::QOBJ_IMMEDIATE_DELETE );
		ctx.Eval( "print( 'object 2 name: '..myobj2.objectName )" );
	    ctx.Eval( "getmetatable( myobj2 ).__gc()" );
		assert( pMyObject.isNull() );
		
		TestObject myobj3;
		ctx.AddQObject( &myobj3, "myobj3", qlua::LuaContext::QOBJ_NO_DELETE );
		ctx.Eval( "print( myobj3.copyString( 'hi' ) );\n"
		          "vm = myobj3.copyVariantMap( {key1=1,key2='hello'} );\n" 
		          "print( vm['key1'] .. ' ' .. vm['key2'] );\n"
				  "print( myobj3.createObject().objectName );" );

        ctx.Eval( "fl = myobj3.copyShortList( {1,2,3} );\n" 
				  "print( fl[1] .. ' ' .. fl[ 3 ] );\n" );

	} catch( const std::exception& e ) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}