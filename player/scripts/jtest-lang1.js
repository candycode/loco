
//Example of how to trace execution of global functions.
//This code be executed before loading html pages to filter function calls.

try {
var Wrapper = { wrap: function(that, v) { 
		                var f = that[ v ];
		                var thisObj = this;
		                //Loco.console.println( "#ARGS: " +  ) 
		                return function() { 
		                  var ret = f.apply( null, arguments );
		                  thisObj.log.push( { name: v, "args": arguments, "return": ret } );
		                  return ret; 
		                } 
		            },
		        log: []
		      };


function sq( x ) { return x * x; };
function cube( x ) { return x * sq(x); };
function sum( a, b ) { return a + b; }

Loco.console.println( "Wrapping following functions with call trace wrapper" );
for( v in this ) {
	try {
      if( typeof this[ v ] === 'function' ) {
	    Loco.console.println( v );
	    this[ v ] = Wrapper.wrap( this, v );
      }
    } catch( e ) {
    	Loco.console.println( "[X] " + v + " -- " + e );
    }  
}

Loco.console.print( "Executing function: 'sum( cube(2), sq(3) )'"  );
Loco.console.println( " = " + sum(cube(2), sq(3)) );

Loco.console.println( "Logged call trace: " );
for( var i = 0;  i != Wrapper.log.length; i += 1 ) {
  var l = Wrapper.log[ i ]
  Loco.console.print( "name: " + l["name"] + ", args: " );
  for( var a = 0; a < l['args'].length; a += 1 ) {
  	Loco.console.print( l['args'][a] );
  	if( a != l['args'].length ) Loco.console.print( "," );
  }
  Loco.console.print( " , return: " + l['return'] );  
  Loco.console.println();
}

} catch( e ) { 
  Loco.console.println( e );
  if( e.stack !== undefined && e.stack !== null ) Loco.console.println( e.stack );
}