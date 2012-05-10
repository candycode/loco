try {

var include = Loco.ctx.include,
    print = Loco.console.println,
    readline = Loco.console.readLine,
    fread = Loco.fs.fread,
    fwrite = Loco.fs.fwrite,
    exit = Loco.ctx.exit,
    ctx = Loco.ctx,
    quit = Loco.ctx.quit,
    HOMEDIR = Loco.ctx.homeDir(),
    APPDIR = Loco.ctx.appDir(),
    CURDIR = Loco.ctx.curDir(),
    WEBKIT = Loco.ctx.jsInterpreterName().indexOf( "webkit" ) >= 0, 
    err = function(msg) { throw msg; },
    CONTEXT_TYPE = WEBKIT ? "JavaScriptCoreContext" : "QtScriptContext";

Loco.ctx.onError.connect( function( msg ) { print( msg ); } );
Loco.ctx.javaScriptConsoleMessage.connect(
 function( msg, line, src ) {
   print( msg + " at line " + line );
 } );
//==============================================================================
var newCtx = Loco.ctx.create( CONTEXT_TYPE );
newCtx.onError.connect( err );
newCtx.javaScriptConsoleMessage.connect(
 function( msg, line, src ) {
   print( msg + " at line " + line );
 } );
print( "Enable storage of source code passed for evaluation" );
newCtx.storeCode = true;
newCtx.addObject( newCtx, "ctx" );
print( "Added new context reference as 'ctx' into new context itself" );
newCtx.addObject( Loco.console, "io" );
print( "Added 'Loco.console' as 'io' into new context" );
var CODE = "io.println(ctx.code)";
print( "Evaluating code '" + CODE + "' in new context" );
newCtx.eval( CODE ); //prints out code passed to newCtx itself!
                     //i.e. 'io.println...'
//==============================================================================
exit(0); //FOR NON-GUI APPS ONLY

} catch( e ) {
  if( e.message ) Loco.console.printerrln( e.message );
  else Loco.console.printerrln( e );
  exit( -1 );
}
