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
    CONTEXT_TYPE = WEBKIT ? "JavaScriptCoreContext" : "QtScriptContext",
    args = Loco.ctx.commandLine;

Loco.ctx.onError.connect( function( msg ) { print( msg ); } );
Loco.ctx.javaScriptConsoleMessage.connect(
 function( msg, line, src ) {
   print( msg + " at line " + line );
 } );

if( args.length < 3 ) {
  print( "Usage: " + args[ 0 ] + " " + args[ 1 ] +
         "<RtAudio plugin file path>" );
  exit( 0 );
}

//==============================================================================
var rtaudio = ctx.loadQtPlugin( args[ 2 ] );
var inbuffer = [];
print( rtaudio.defaultOutputDevice );
rtaudio.error.connect( err );
rtaudio.filter.connect( function( input ) {
  var l = input.length;
  var output = [];
  for( var i = 0; i !== l; ++i ) {
    output.push( input[ i ] * Math.sin( ( 100* 2 * Math.PI * i ) / l ) );
  }
  rtaudio.setOutput( output );
} );
rtaudio.openIOStream( {}, {}, "float64", 4000, 500 );

//==============================================================================
//invoke exit() event loop has not been started, quit() otherwise
//locoplay: event loop is created by default, to disable invoke with -nl flag:
//locoplay -nl ...
//exit(0); 

} catch( e ) {
  if( e.message ) Loco.console.printerrln( e.message );
  else Loco.console.printerrln( e );
  exit( -1 ); // will generate WaitCondition error message if messag loop active
}



