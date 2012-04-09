//key, certificate generated with the following command line:
// openssl req -x509 -nodes -days 365 -newkey rsa:1024 -keyout \
// test28-key.pem -out test28-cert.pem
/*
Country Name (2 letter code) [AU]:CH
State or Province Name (full name) [Some-State]:Zuerich
Locality Name (eg, city) []:Zuerich
Organization Name (eg, company) [Internet Widgits Pty Ltd]:LoCO
Organizational Unit Name (eg, section) []:loco-test
Common Name (eg, YOUR name) []:loco
Email Address []:loco@locojs.net
*/

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
// from within a separate thread
function encryptedFortune() {
  var p = terminal.println;
  p( "\n\n=========================================\n\n" );
  p( "Thread " + thisThread.id ); 
  p( "connected" );
  var tcpSocket = net.create( "tcp-ssl-socket" );
  // 'socket' is initialized by the parent context before
  // this function is invoked
  if( !tcpSocket.setSocketDescriptor( socket ) ) {
    p( "Cannot create socket" );
    return;
  }
  p( "Setting certificate:\n" );
  tcpSocket.setLocalCertificateFromFile( "test28-ssl/test28-cert.pem" );
  p( tcpSocket.getLocalCertificate() );
  p( "Setting key:\n" );
  tcpSocket.setPrivateKeyFromFile( "test28-ssl/test28-key.pem", "rsa" );
  p( tcpSocket.getPrivateKey() );
  tcpSocket.encrypted.connect( function() { p("ENCRYPTED"); } );
  tcpSocket.startServerEncryption( 1 );
  if( tcpSocket.isConnected() ) p( "Created encrypted connection" );
  p( "  remote host: " + tcpSocket.peerAddress );
  p( "  port:        " + tcpSocket.peerPort );
  // taken from Qt's 'fortune' sample code
  var fortunes = ["You've been leading a dog's life. Stay off the furniture.",
                  "You've got to think about tomorrow.",
                  "You will be surprised by a loud noise.",
                  "You will feel hungry again in another hour.",
                  "You might have mail.",
                  "You cannot kill time without injuring eternity.",
                  "Computers are not intelligent. They only think they are."];
  
  //tcpSocket.send( fortunes[ Math.floor( Math.random() * fortunes.length ) ] );
  // above line equivalent to following two
  tcpSocket.write( fortunes[ Math.floor( Math.random() * fortunes.length ) ] );
  tcpSocket.waitForBytesWritten( 2000 );

  p("Message sent to client");
  
  tcpSocket.close(); //will flush
 
};

include( "cmdline2json.js" );

var cmdLine = cmdLineToJSON( Loco.ctx.commandLine );

if( !cmdLine[ "-port" ] ) throw "Error '-port' required\nUsage: test28-tcp.js [-server] -port #";
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
      thread.context.eval( "socket = " + socket );
      thread.execute( encryptedFortune ); //.toString() automatically called
  } );
  if( !tcpServer.listen( cmdLine[ "-port" ]  ) ) throw "Listen error";
  print( "Listening on port " + cmdLine[ "-port" ] ); 
} else {
    var tcp = Loco.net.create( "tcp-ssl-socket" );
    //tcp.ignoreSSLErrors(); //should not be needed
    // without a proper configuration you'll get: 
    //"The host name did not match any of the valid hosts for this certificate"
    tcp.setPeerVerifyMode( "none" );
    tcp.connectTo( "127.0.0.1", cmdLine[ "-port" ], 2000 );
    print( tcp.readAll() );
    tcp.close();
    exit( 0 );
}
//==============================================================================
} catch( e ) {
  if( e.message ) Loco.console.printerrln( e.message );
  else Loco.console.printerrln( e );
  exit( -1 );
}

