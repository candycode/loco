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

function fortune() {
  var fortunes = ["You've been leading a dog's life. Stay off the furniture.",
                  "You've got to think about tomorrow.",
                  "You will be surprised by a loud noise.",
                  "You will feel hungry again in another hour.",
                  "You might have mail.",
                  "You cannot kill time without injuring eternity.",
                  "Computers are not intelligent. They only think they are."];
  //net.tcpSend( socket, fortunes[ Math.random() * fortunes.length ] );
};

include( "cmdline2json.js" );

var cmdLine = cmdLineToJSON( Loco.ctx.commandLine );

if( !cmdLine[ "-port" ] ) throw "Error '-port' required";
if( cmdLine[ "-server" ] ) {
  var tcpServer = Loco.net.create( "tcp-server" );
  if( !tcpServer.listen( "127.0.0.1", cmdLine[ "-port" ]  ) ) throw "Listen error";
  tcpServer.connectionRequest.connect( function( socket ) {
      print( "connected" );
      var thread = Loco.sys.create( "ContextThread" );
      thread.addObject( Loco.net, "net" );
      thread.context = Loco.ctx.create( "QtScriptContext" );
      thread.context.eval( "socket = " + socket );
      thread.execute( fortune );
    } );
    print( "Listening on port " + cmdLine[ "-port" ] ); 
} else {
    /*var tcp = Loco.net.create( "tcp-socket" );
    tcp.connectTo( "localhost", cmdLine[ "-port" ], "read" );
    print( tcp.recv() );
    tcp.disconnectFromHost();*/
}
//==============================================================================
//exit(0); //FOR NON-GUI, NON-NETWORK-SERVER APPS ONLY

} catch( e ) {
  if( e.message ) Loco.console.printerrln( e.message );
  else Loco.console.printerrln( e );
  exit( -1 );
}

