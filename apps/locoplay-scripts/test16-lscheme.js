var print = Loco.console.println;
var WKIT = Loco.ctx.jsInterpreterName().indexOf( "webkit" ) >= 0;
var read = Loco.ctx.read;
var exit = Loco.ctx.exit;

try { 
  if( WKIT ) {
    Loco.ctx.include( "../../filters/lscheme/little.js" );
    print = alert;
  }
  else eval( read( "../../filters/lscheme/little.js" ) );
  p(value(s( "(print ( quote( ciao)  ) )") ) );
  exit( 0 );
} catch( e ) {
  print( e );
  exit( -1 ); 
}
