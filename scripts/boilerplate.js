try {

var include = Loco.ctx.include,
    print = Loco.console.println,
    readline = Loco.console.readLine,
    fread = Loco.ctx.read,
    exit = Loco.ctx.exit,
    WEBKIT = Loco.ctx.jsInterpreterName().indexOf( "webkit" ) >= 0, 
    err = function(msg) { throw err; }

Loco.ctx.onError.connect( function( msg ) { print( msg ); } );
//==============================================================================
  
//<your code here>  

//==============================================================================
exit(0);

} catch( e ) {
  if( e.message ) Loco.console.printerrln( e.message );
  else Loco.console.printerrln( e );
  exit( -1 );
}    
