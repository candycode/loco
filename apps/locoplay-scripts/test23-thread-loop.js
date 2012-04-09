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

function createThreads( numThreads, storage ) {
  var threads = [],
      threadContext,
      i;

  for( i = 0; i != numThreads; ++i ) {
    threadContext = ctx.create( "QtScriptContext" );
    threadContext.addObject( Loco.console, "terminal" );
    threadContext.addObject( storage, "storage" );
    threadContext.addObject( Loco.sys, "sys" );
    threadContext.eval( "var inarray = storage.data.array;/*.splice(0)*/" );
    threadContext.onError.connect( err );
    var p = threads.push( ctx.create( "ContextThreadLoop" ) ) - 1;
    threads[ p ].setContext( threadContext );
    threadContext.addObject( threads[ p ], "me" );
  }
  return threads;
}

function createArray( size, f ) {
  var array = [];
  for( var i = 0; i != size; ++i ) array.push( f( i ) );
  return array;
}

var i = 0;
var SIZE = 4096 * 4096;
var array = createArray( SIZE, function( v ){ return 2 * v; } );
var NUM_THREADS = 4;
var storage = ctx.data();
storage.data = {"array": array};
var threads = createThreads( NUM_THREADS, storage );
var SPAN = array.length / NUM_THREADS;


print( "Number of logical processors: " + Loco.sys.cpuCount() );
print( "Main thread id: " + Loco.sys.threadId() );
print( threads.length + " threads" );

var threadFun =
"function( datain ) { \
  var clocks = sys.clock(); \
  var outarray = new Array( datain.span ); \
  var start = datain.id * datain.span, \
      end = start + datain.span - 1; \
  for( var i = start, j = 0; i != end; ++i, ++j ) { \
    outarray[ j ] = 2 * inarray[ i ]; \
  } \
  me.data = outarray; \
  return ( sys.clock() - clocks ) / sys.CLOCKS_PER_SECOND; \
}";

// explicitly start loops
//for( i = 0; i != NUM_THREADS; ++i ) {
//  print( threads[ i ].running() );
//  threads[ i ].startLoop(); // guarantees that the thread starts and waits at the wait condition 
//  print( threads[ i ].running() );
//}

// start asynchronous evaluation on mutliple threads
// startLoop() is automatically called if thread not yet started
for( i = 0; i != NUM_THREADS; ++i ) {
  threads[ i ].eval( "(" + threadFun + ")" + "("+"{id:"+i+",span:"+SPAN+"}"+")" );
}

// explicitly sync
//for( i = 0; i != NUM_THREADS; ++i ) {
//  threads[ i ].sync();
//}

// future-like behavior: synchronization performed if/when required at data access time
var outarray = new Array( NUM_THREADS );
var times = new Array( NUM_THREADS );
for( i = 0; i != NUM_THREADS; ++i ) {
  outarray[ i ] = threads[ i ].data[ 0 ];
  times[ i ]    =  threads[ i ].result;
}
 
for( i = 0; i != NUM_THREADS; ++i ) {
  print( "Thread " + i + "(" + threads[ i ].id + ")\t elapsed time: " + 
         times[ i ] + "\tarray[0] = " + outarray[ i ] );
}

// stop threads
//for( i = 0; i != NUM_THREADS; ++i ) {
//  //stop thread i: wait up to 5000 micro seconds (5 ms) for the thread to stop
//  if( !threads[ i ].stopLoop( 5000 ) ) throw "Error stopping thread " + i;
//}

// recreate environment found in threads
var me = {};
var inarray = storage.data.array;
var sys = Loco.sys;
print( "\nsingle thread" );
var elapsed = ctx.eval( "(" + threadFun + ")" + "("+"{id:"+0+",span:"+inarray.length+"}"+")" );
for( i = 0; i != NUM_THREADS; ++i ) {
  print( "array[" + SPAN * i + "] = " + me.data[ SPAN * i ] );
} 
print( "elapsed time: " + elapsed );

//==============================================================================
exit(0); //FOR NON-GUI APPS ONLY

} catch( e ) {
  if( e.message ) Loco.console.printerrln( e.message );
  else Loco.console.printerrln( e );
  exit( -1 );
}



