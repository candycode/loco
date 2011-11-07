var print = Loco.console.println;
var exit = Loco.ctx.exit;
var include = Loco.ctx.include;
var read = Loco.ctx.read;
var addFilter = Loco.ctx.addScriptFilter;
try {
  Loco.ctx.onError.connect( function( e ) { throw e; } ); 
  include( "../filters/skulpt/skulpt.js" );
  include( "../filters/skulpt/builtin.js" );
  include( "../filters/skulpt/skulpt-filter.js" );
  /*USE BUILTINS TO PASS FUNCTIONS AROUND:
  Sk.builtin.webWindow = function( s ) {
    var WW = Loco.gui.create( "WebWindow" );
    WW.load( s.v );
    WW.show();
  }
  Sk.builtins[ "load" ] = Sk.builtin.load;*/
  addFilter( "skulpt", "loco_skulptCompile_",
						         "function loco_skulptCompile_( code ) {" +
						         "  var s = Sk.compile( code, '', 'single' );" +
						         "  return s.code + s.funcname + '()';" +
						         "}" );
  include( "./fibonacci.py", ["skulpt"] );
  exit( 0 ); 
} catch( e ) {
  print( e );
  exit( -1 );
}
