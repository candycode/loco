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

include(':/json2.js');
include('../../filters/haml.js');
print_date = function () { return 'today: ';};
var ww = Loco.gui.create( 'WebWindow' );
var hamlText = Loco.ctx.read( 'test25-haml.haml' );
var current_user = { address: 'my address', email: 'e@ma.il', bio: 'bio', hamlCode: hamlText };
ww.setAttributes( {DeveloperExtrasEnabled: true} );                  
ww.setEnableContextMenu( true );
//'current_user' field names are substituted with values defined in this context
ww.setHtml( Haml.render( hamlText ) );
ww.show();

//==============================================================================
//exit(0); //FOR NON-GUI APPS ONLY

} catch( e ) {
  if( e.message ) Loco.console.printerrln( e.message );
  else Loco.console.printerrln( e );
  exit( -1 );
}



