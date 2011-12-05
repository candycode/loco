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
var socket = Loco.net.create( "tcp-ssl-socket" );
socket.connectTo( "bugs.kde.org", 443, 5000 );
//if ( !socket.waitForEncrypted( 50000 ) ) throw socket.errorMsg();
socket.write( "GET / HTTP/1.1\r\n"+
              "Host: bugs.kde.org\r\n"+
              "Connection: Close\r\n\r\n" );
var data = "";
while( socket.waitForReadyRead( 5000 ) ) data += socket.readAll();
var suffixLength = 5;
print( data.slice(0,data.length-suffixLength) ); //remove non printable chars
print( "done" );
//==============================================================================
exit(0); //if event loop not active

} catch( e ) {
  if( e.message ) Loco.console.printerrln( e.message );
  else Loco.console.printerrln( e );
  exit( -1 );
}



