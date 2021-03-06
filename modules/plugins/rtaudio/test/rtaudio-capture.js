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
rtaudio.error.connect( err );
var input = [];
rtaudio.inputReady.connect( function() {
  input = rtaudio.input;
  var l = input.length;
  for( var i = 0; i !== l; ++i ) Loco.console.println( input[ i ] );
} );
rtaudio.openInputStream( "float64", 11025, 256 );
rtaudio.startStream();

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



