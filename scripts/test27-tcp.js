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

// this function is parsed and executed in a separate child context
// inside a separate thread
function fortune() {
  terminal.println( "Thread " + thisThread.id ); 
  terminal.println( "connected" );
  var tcpSocket = net.create( "tcp-socket" );
  tcpSocket.socket = socket;
  terminal.println( "  remote host: " + tcpSocket.peerAddress );
  terminal.println( "  port:        " + tcpSocket.peerPort );   
  // taken from Qt's 'fortune' sample code
  var fortunes = ["You've been leading a dog's life. Stay off the furniture.",
                  "You've got to think about tomorrow.",
                  "You will be surprised by a loud noise.",
                  "You will feel hungry again in another hour.",
                  "You might have mail.",
                  "You cannot kill time without injuring eternity.",
                  "Computers are not intelligent. They only think they are."];
  tcpSocket.send( fortunes[ Math.floor( Math.random() * fortunes.length ) ] );
  tcpSocket.flush();
  tcpSocket.close();
};

include( "cmdline2json.js" );

var cmdLine = cmdLineToJSON( Loco.ctx.commandLine );

if( !cmdLine[ "-port" ] ) throw "Error '-port' required";
if( cmdLine[ "-server" ] ) {
  var tcpServer = Loco.net.create( "tcp-server" );
  var arr = new ArrayBuffer( 256 );
  var v = new Float32Array( arr );
  v[ 0 ] = 0.123234;
  tcpServer.testArrayBuffer( v );
  tcpServer.connectionRequest.connect( function( socket ) {
      var thread = Loco.ctx.create( "ContextThread" );
      thread.autoDestroy = true; 
      var context = Loco.ctx.create( "QtScriptContext" );
      context.addObject( Loco.net, "net" );
      context.addObject( Loco.console, "terminal" );
      context.addObject( thread, "thisThread" );
      context.eval( "socket = " + socket + ";" );
      thread.setContext( context );
      thread.execute( fortune );
  } );
  if( !tcpServer.listen( cmdLine[ "-port" ]  ) ) throw "Listen error";
  print( "Listening on port " + cmdLine[ "-port" ] ); 
} else {
    var tcp = Loco.net.create( "tcp-socket" );
    tcp.connectTo( "localhost", cmdLine[ "-port" ], 3000, "r" );
    tcp.close();
}
//==============================================================================
if( !cmdLine[ "-server" ] ) exit(0); //FOR NON-GUI, NON-NETWORK-SERVER APPS ONLY

} catch( e ) {
  if( e.message ) Loco.console.printerrln( e.message );
  else Loco.console.printerrln( e );
  exit( -1 );
}

