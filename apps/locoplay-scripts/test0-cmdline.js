//loco test0-cmdline.js -first first -second s1 -second s2 -third 1 2 3

try {
var include = Loco.ctx.include,
    print = Loco.console.println,
    exit = Loco.ctx.exit;
    
Loco.ctx.onError.connect( function( msg ) { print( msg ); } );
Loco.ctx.javaScriptConsoleMessage.connect(
 function( msg, line, src ) {
   print( msg + " at line " + line );
 } );
//==============================================================================
include("cmdline2json.js");
var cmdLine = cmdLineToJSON( Loco.ctx.commandLine );
for( var c in cmdLine ) {
  Loco.console.print( c + ": " );
  var values = cmdLine[ c ];
  for( var i = 0; i != values.length; ++i ) Loco.console.print( values[ i ] + " " );
  print(); 
}
//==============================================================================
//invoke exit() event loop has not been started, quit otherwise
//locoplay: event loop is created by default, to disable invoke with -nl flag:
//locoplay -nl ...
exit(0); 

} catch( e ) {
  if( e.message ) Loco.console.printerrln( e.message );
  else Loco.console.printerrln( e );
  exit( -1 );
}



