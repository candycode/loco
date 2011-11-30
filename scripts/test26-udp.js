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
include( "cmdline2json.js" );
var cmdLine = cmdLinetoJson( Loco.ctx.cmdLine );
if( !cmdLine[ "-port" ] ) throw "Error '-port' required";
var udp = Loco.net.create( "udp-socket" );
var ack = "";
if( cmdLine[ "-server" ] ) {
  udp.bind( "localhost", cmdLine[ "-port" ], "DefaultForPlatform" );
  udp.recv.connect( function( data, sender, senderPort ) {
      print( data );
      udp.send( "ok" );
    } );
} else {
  while( true ) {
    udp.send( readLine(), "localhost", cmdLine[ "-port" ] );
    ack = udp.recv( "localhost", cmdLine[ "-port" ] );  
  }
  // OR
  //udp.connect( "localhost", cmdLine[ "-port" ], "write" );
  //while( true ) {
  //  udp.send( readLine() );
  //  ack = udp.recv();
  //  if( ack !== 'ok' ) throw "Communication error";  
  //}
}
//==============================================================================
//exit(0); //FOR NON-GUI, NON-NETWORK-SERVER APPS ONLY

} catch( e ) {
  if( e.message ) Loco.console.printerrln( e.message );
  else Loco.console.printerrln( e );
  exit( -1 );
}

