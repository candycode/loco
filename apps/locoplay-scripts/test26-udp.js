//usage: locoplay test25-udp.js -inport <in> -outport <out> -host <host>
//sends messages to output port, receive messages from input port
//E.g.
//Terminal #1:
// > locoplay test25-udp.js -inport 12331 -outport 12333 -host 127.0.0.1
//Terminal #2:
// > locoplay test25-udp.js -inport 12333 -outport 12331 -host 127.0.0.1
// > 'Connected, waiting for message'
// Terminal #1:
// > 'hello' 
// hey
// Terminal #2:
// > hey


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
//==============================================================================
include( "cmdline2json.js" );
var handShakeCompleted = false;
var ACK = "-ack-";
var START = "-start-";
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
   var msg = udp.recvFrom( host, inport );
   if( !handShakeCompleted ) {      
      if( msg === START ) {
        udp.sendTo( ACK, host, outport );
        handShakeCompleted = true;                   
      } else if( msg === ACK ) {
        handShakeCompleted = true;
        print( "Connected, waiting for message" );
        udp.sendTo( "hello", host, outport );
       } else {
        throw "Wrong handshake protocol: " + 
               ACK + " or " + START + " expected, " + msg + " received"; 
      }      
    } else {
      print( "> " + msg );
      udp.sendTo( readline(), host, outport );
    }
  } ); 
udp.bind( "127.0.0.1", inport );
udp.sendTo( START, host, outport );
//==============================================================================
//exit(0); //FOR NON-GUI, NON-NETWORK-SERVER APPS ONLY
} catch( e ) {
  if( e.message ) Loco.console.printerrln( e.message );
  else Loco.console.printerrln( e );
  exit( -1 );
}

