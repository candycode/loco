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
rtaudio.error.connect( err );
var sampleCount = 0;
var sampleBuffer = 512;
var sampleRate = 44100;
var maxSampleCount = sampleRate - 1;
var output = new Array( sampleBuffer );
var input = [];
var F = 100; //Hz
function filter( t, idx, input ) {
  var N = 100;
  var out = 0;
  for( var i = 0; i != N; ++i ) {
    out += ( 0.5 * ( 1 + Math.cos( 2 * Math.PI * F * i * t + i / N ) ) ) / N;
  } 
  return out * input[ idx ];  
}
rtaudio.filter.connect( function() {
  input = rtaudio.input;
  var l = input.length;
  for( var i = 0; i !== l; ++i ) {
    ++sampleCount; if( sampleCount ===  maxSampleCount ) sampleCount = 0;
    output[ i ] = filter( sampleCount / maxSampleCount,  i, input );
  }
  rtaudio.output = output;
} );
rtaudio.openIOStream( "float64", sampleRate, sampleBuffer );
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



