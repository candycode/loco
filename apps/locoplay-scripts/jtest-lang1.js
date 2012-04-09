
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
		        log: [],
		        toString: function() {
		        	var text;
		        	for( var i = 0;  i < this.log.length; i += 1 ) {
                      var l = this.log[ i ];
                      text += "name: " + l["name"] + ", args: ";
                      for( var a = 0; a < l['args'].length; a += 1 ) {
  	                    text += l['args'][a];
  	                    if( a !== l['args'].length ) text += ",";
                      }
                      text += " , return: " + l['return'];  
                      text += "\n";
                    }
                    return text;
		        }
		      };


function sq( x ) { return x * x; };
function cube( x ) { return x * sq(x); };
function sum( a, b ) { return a + b; }

for( v in this ) { 
                try { 
                  if( typeof this[ v ] === "function" ) { 
                    this[ v ] = Wrapper.wrap( this, v ); 
                  } 
                } catch( e ) { 
                Loco.console.println( "[X] " + v + " -- " + e ); 
              } 
             }; 

Loco.console.print( "Executing function: 'sum( cube(2), sq(3) )'"  );
Loco.console.println( " = " + sum(cube(2), sq(3)) );

Loco.console.println( "Logged call trace: " );

Loco.console.println( Wrapper.toString() );

} catch( e ) { 
  Loco.console.println( e );
  if( e.stack !== undefined && e.stack !== null ) Loco.console.println( e.stack );
}