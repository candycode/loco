//var print = Loco.console.println;
var WKIT = Loco.ctx.jsInterpreterName().indexOf( "webkit" ) >= 0;
var read = Loco.ctx.read;
var exit = Loco.ctx.exit;

try { 
  if( WKIT ) Loco.ctx.include( "../filters/lscheme/little.js" );
  else eval( read( "../filters/lscheme/little.js" ) );
  p(value(s("(print (quote(ciao) ) )") ) );
  Loco.ctx.addScriptFilter( "lscheme", "value(s($))", "", "", "$" );
  Loco.ctx.evalFile( "test16.lscheme", ["lscheme"] );
  exit( 0 );
} catch( e ) {
  print( e );
  exit( -1 ); 
}
