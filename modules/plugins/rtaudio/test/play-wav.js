try {
var include = Loco.ctx.include,
    print = Loco.console.println,
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

function resample( data, inRate, outRate /*, bool round*/ ) {
  var r = outRate / inRate;
  function lintp( start, end, t ) {
    var f = data[ start ] + ( data[ end ] - data[ start ] ) * t;
    return /*round ? Math.round( f ) :*/ f; 
    //no need to convert to int since data is passed back to C++
    //as doubles
  }
  var totalLength = Math.floor( data.length * r );
  var out = new Array( totalLength );
  for( i = 0; i != totalLength - 1; ++i ) {
    // r == ratio == sample-axis interval:
    // sample interval of original signal == d1 == 1
    // sample interval of resampled signal == d2 == d1 * r == r 
    out[ i ] = lintp( Math.floor( i / r ), Math.ceil( i / r ), i/r - Math.floor( i / r ) );
  }  
  return out;
}

function mergeChannels() {
  if( arguments.length < 1 ) return undefined;
  var out = new Array( arguments[ 0 ].length * arguments.length );
  var ch = arguments.length;
  var samples = arguments[ 0 ].length;
  var i, j;
  for( i = 0; i != samples; ++i ) {
    for( j = 0; j != ch; ++j ) {
      out[ ch * i + j ] = arguments[ j ][ i ];
    }
  }
  return out;
}

function splitChannels( data, ch ) {
  var length = data.length;
  if( ch === undefined ) throw "Missing number of channels";
  if( length % ch !== 0 ) {
    throw "Datasize is not a multiple of the number of channels";
    return null;
  }
  var samples = length / ch;
  var out = new Array( ch );
  var i, j;
  for( i = 0; i != ch; ++i ) out[ i ] = new Array( samples );
  for( i = 0; i < samples; ++i ) {
    for( j = 0; j != ch; ++j ) {
      out[ j ][ i ] = data[ i * ch + j ];
    } 
  } 
  return out;
}

var sinwave = signal( 32767, 1, 44100, 400, 0);
var rtaudio = ctx.loadQtPlugin( args[ 2 ] );
rtaudio.error.connect( err );
var wav = rtaudio.readFile( args[ 3 ], true );
wav.data = resample( wav.data, wav.rate, 44100 );
print( wav.data.length + " " + wav.data.length / 44100 );
wav.data = mergeChannels( wav.data, wav.data );
sinwave = mergeChannels( sinwave, sinwave ); 
print( "Data type: " + wav.type + 
       "\nSampling rate: " + wav.rate +
       "\nChannels: " + wav.numChannels +
       "\nNumber of samples: " + wav.data.length );
wav.numChannels = 2;
wav.rate = 44100;
play( rtaudio, wav.data, 2, "float64", 
      function() { 
        play( rtaudio, sinwave, 2, "sint16", 
              function() { 
                rtaudio.writeFile( "out.wav", { data: wav.data, type: "sint16", numChannels: 2 }  );
                quit(); 
              } ) 
      } ); 


//==============================================================================

} catch( e ) {
  if( e.message ) Loco.console.printerrln( e.message );
  else Loco.console.printerrln( e );
  exit( -1 ); // will generate WaitCondition error message if message loop active
}



