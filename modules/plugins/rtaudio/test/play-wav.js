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
         "<RtAudio plugin file path> <wav file path>" );
  exit( 0 );
}

//==============================================================================

function play( audio, signal, channels, type, finish ) {
  
  function cback() {
    audio.output = signal.slice( offset, offset + samples * channels );
    if( offset < ( signal.length - samples * channels ) ) offset += samples * channels;
    else {
      audio.stopStream();
      audio.closeStream();
      audio.outputReady.disconnect( cback );
      if( finish ) finish();
    }
  }

  var offset = 0;
  var samples = 256;
  audio.outputReady.connect( cback );
  if( audio.isStreamOpen() ) audio.closeStream();
  audio.openOutputStream( type, 
                          audio.sampleRate, 
                          samples,
                          { deviceId: rtaudio.defaultOutputDevice,
                            numChannels: channels } );
  audio.startStream();
}


function signal( S, d, sr, f, phi ) {
  var data = [];
  phi = phi || 0.0; 
  var n = d * sr;
  for( var i = 0; i != n; ++i ) data.push( S*Math.sin( f*2*Math.PI*i/sr + phi ) );
  return data;  
}

function resample( data, inRate, outRate, ch ) {
  var r = outRate / inRate;
  ch = ch || 1;
  var step = Math.round( r * ch );
  var chstep = Math.round( r ); 
  var totalLength = Math.ceil( data.length * r );
  var length = Math.floor( data.length * r );
  var out = new Array( totalLength );
  var i, j, k, c;
  for( i = 0, j = 0; i < ( length - ch ); ++i, j += step ) {
    start = data[ i ];
    end = data[ i + ch ];
    for( k = 0, c = 0; k < step; k += ch, ++c ) {
      out[ j + k ] = Math.round( start + (end - start) * c / chstep );
    }
  }
  for( i = length - ch; i < totalLength; i += ch ) {
    for( j = 0; j != ch; ++j ) out[ i + j ] = out[ length - ch + j ];
  } 
  return out;
}

function merge() {
  print( arguments.length );
  if( arguments.length < 1 ) return undefined;
  var out = new Array( arguments[ 0 ].length * arguments.length );
  var ch = arguments.length;
  var samples = arguments[ 0 ].length;
  var i, j;
  for( i = 0; i != out.length; i += ch ) {
    for( j = 0; j != ch; ++j ) {
      out[ i + j ] = arguments[ j ][ i / ch ];
    }
  }
  return out;
}


var sinwave = signal( 32767, 1, 44100, 400, 0);
var rtaudio = ctx.loadQtPlugin( args[ 2 ] );
rtaudio.error.connect( err );
var wav = rtaudio.readFile( args[ 3 ] );
wav.data = resample( wav.data, wav.rate, rtaudio.sampleRate, wav.numChannels );
wav.data = merge( wav.data, wav.data ); 
print( "Data type: " + wav.type + 
       "\nSampling rate: " + wav.rate +
       "\nChannels: " + wav.numChannels +
       "\nNumber of samples: " + wav.data.length );
rtaudio.sampleRate = 44100;
play( rtaudio, wav.data, 2, wav.type, function() { play( rtaudio, sinwave, 1, "sint16", function(){ Loco.ctx.quit(); } ) } ); 


//==============================================================================
//invoke exit() event loop has not been started, quit() otherwise
//locoplay: event loop is created by default, to disable invoke with -nl flag:
//locoplay -nl ...
//exit(0); 

} catch( e ) {
  if( e.message ) Loco.console.printerrln( e.message );
  else Loco.console.printerrln( e );
  exit( -1 ); // will generate WaitCondition error message if message loop active
}



