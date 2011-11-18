try {

var include = Loco.ctx.include,
    print = Loco.console.println,
    readline = Loco.console.readLine,
    fread = Loco.fs.fread,
    exit = Loco.ctx.exit,
    ctx = Loco.ctx,
    quit = Loco.ctx.quit,
    HOMEDIR = Loco.ctx.homeDir(),
    APPDIR = Loco.ctx.appDir(),
    CURDIR = Loco.ctx.curDir(), 
    WEBKIT = Loco.ctx.jsInterpreterName().indexOf( "webkit" ) >= 0, 
    err = function(msg) { throw msg; }

Loco.ctx.onError.connect( function( msg ) { print( msg ); } );
//==============================================================================
  
//<your code here>  

//==============================================================================
//exit(0); //FOR NON-GUI APPS ONLY

} catch( e ) {
  if( e.message ) Loco.console.printerrln( e.message );
  else Loco.console.printerrln( e );
  exit( -1 );
}    
