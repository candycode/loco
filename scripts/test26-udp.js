try {
var include = Loco.ctx.include,
    print = Loco.console.println,
    readline = Loco.console.readLine,
    exit = Loco.ctx.exit,
    ctx = Loco.ctx,
    quit = Loco.ctx.quit;
   
Loco.ctx.onError.connect( function( msg ) { print( msg ); } );
Loco.ctx.javaScriptConsoleMessage.connect(
 function( msg, line, src ) {
   print( msg + " at line " + line );
 } );

//change logic:
// 1) try to send 'ready' message to other host
// 2) if (1) fails start listening
//==============================================================================
include( "cmdline2json.js" );
var cmdLine = cmdLineToJSON( Loco.ctx.commandLine );
var host, inport, outport;
if( !cmdLine[ "-inport" ] ) throw "Error '-inport' required";
else inport = cmdLine[ "-inport" ];
if( !cmdLine[ "-outport" ] ) throw "Error '-outport' required";
else outport = cmdLine[ "-outport" ];  
if( !cmdLine[ "-host" ] ) throw "Error '-host' required";
else host = cmdLine[ "-host" ];
var udp = Loco.net.create( "udp-socket" );
udp.readyRead.connect( function() {
    print( udp.recvFrom( host, inport ) ); 
    udp.sendTo( readline(), host, outport );
  } ); 
udp.bind( "127.0.0.1", inport );
if( cmdLine[ "-talk-first" ] ) {
  udp.sendTo( readline(), host, outport );
}
//==============================================================================
//exit(0); //FOR NON-GUI, NON-NETWORK-SERVER APPS ONLY
} catch( e ) {
  if( e.message ) Loco.console.printerrln( e.message );
  else Loco.console.printerrln( e );
  exit( -1 );
}

