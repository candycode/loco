try {
var include = Loco.ctx.include,
    print = Loco.console.println,
    exit = Loco.ctx.exit,
    ctx = Loco.ctx;

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
  var tcpSocket = net.create( "tcp-ssl-socket" );
  tcpSocket.setSocketDescriptor( socket );
  tcpSocket.startServerEncryption( 3000 );
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

if( !cmdLine[ "-port" ] ) throw "Error '-port' required\nUsage: test27-tcp.js [-server] -port #";
if( cmdLine[ "-server" ] ) {
  var tcpServer = Loco.net.create( "tcp-server" );
  tcpServer.connectionRequest.connect( function( socket ) {
      var thread = Loco.ctx.create( "ContextThread" );
      thread.autoDestroy = true; 
      thread.context = Loco.ctx.create( "QtScriptContext" );
      // net module must be copied inside the child context
      // to guarantee that sockets are created inside the child
      // context, if only a reference is passed then the created
      // objects live inside the parent (this) context
      thread.context.addNewObject( Loco.net, "net" );
      thread.context.addObject( Loco.console, "terminal" );
      thread.context.addObject( thread, "thisThread" );
      thread.context.eval( "socket = " + socket + ";" );
      thread.execute( fortune );
  } );
  if( !tcpServer.listen( cmdLine[ "-port" ]  ) ) throw "Listen error";
  print( "Listening on port " + cmdLine[ "-port" ] ); 
} else {
    var tcp = Loco.net.create( "tcp-ssl-socket" );
    tcp.connectTo( "localhost", cmdLine[ "-port" ], 3000 );
    print( tcp.recv( 3000 ) );
    tcp.close();
}
//==============================================================================
if( !cmdLine[ "-server" ] ) exit(0); //FOR NON-GUI, NON-NETWORK-SERVER APPS ONLY

} catch( e ) {
  if( e.message ) Loco.console.printerrln( e.message );
  else Loco.console.printerrln( e );
  exit( -1 );
}

