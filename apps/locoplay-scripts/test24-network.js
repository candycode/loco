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

var http = Loco.net.create( "Http" );
var reply = http.get( "http://www.marinij.com", 10000 );
var headers = "";
for( var h in reply.headers ) {
  headers +=  h + ": ";
  if( reply.headers[ h ] ) headers += reply.headers[ h ];
  headers += "\n";
}
if( headers.length > 0 ) fwrite( "test24-output-headers.txt", headers );
fwrite( "test24-output.html", reply.content );
if( Loco.gui ) {
  var ww = Loco.gui.create( "WebWindow" );
  ww.load( "test24-output.html" ); 
  ww.show();
}

//==============================================================================
if( !Loco.gui ) exit(0);

} catch( e ) {
  if( e.message ) Loco.console.printerrln( e.message );
  else Loco.console.printerrln( e );
  exit( -1 );
}



