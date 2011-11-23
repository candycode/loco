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
var i = 0;
var array = [];
for( i = 0; i != 100; ++i ) array.push(i);
var storage = ctx.data();
storage.data = {"array": array};
var runner = Loco.sys.runner();
var futures = [];
var contexts = [];
var thread = 
"function x2( datain ) { \
    console.println('!'); \
    var start = datain.id * datain.span, \
        end = start + datain.span - 1; \
    for( var i = start; i != end; ++i ) { \
      console.println( datain.id ); \
      storage.data.array[ i ] = 2 * storage.data.array[ i ]; \
    } \
    return true; \
};";

var MAX_THREADS = 1;
for( i = 0; i != MAX_THREADS; ++i ) {
  contexts.push( ctx.create( "JavaScriptCoreContext" ) );
  contexts[ i ].addObject( Loco.console, "console" ); 
  contexts[ i ].addObject( storage, "storage" );
  contexts[ i ].eval( thread );
  futures[ i ] = runner.run( "x2( {id:" + i + ", span:" + 25 +"});true;", contexts[ i ] );
}
var done = futures[ 0 ].result;
for( i = 1; i != futures.length; ++i ) {
  done && futures[ i ].result;
}
print( "done = " + done );
//==============================================================================
//exit(0); //FOR NON-GUI APPS ONLY

} catch( e ) {
  if( e.message ) Loco.console.printerrln( e.message );
  else Loco.console.printerrln( e );
  exit( -1 );
}



