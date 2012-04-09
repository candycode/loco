// copy 1M array to c++ and back from c++: 165 ms
// invoking a js signal from a c++ method and passing 1M elements: 170 ms
// 10k elements: 1.6 ms copy, 1.6 ms invocation 

try {
if( Loco.ctx.commandLine.length !== 3 ) {
  Loco.console.println( "Usage: " + 
                        Loco.ctx.commandLine[ 0 ] + " " +
                        Loco.ctx.commandLine[ 1 ] + " <benchmark plugin file path>)" );
  Loco.ctx.exit( 0 ); 
}
var pluginPath = Loco.ctx.commandLine[ 2 ];

var timer = Loco.sys.timer();
var counter = 0;
var benchmark = Loco.ctx.loadQtPlugin( pluginPath );
var SIZE = 10000;//00;
var array = new Array( SIZE );
for( var i = 0; i != SIZE; ++i ) {
  array[ i ] = i;
}
var newarray = [];
timer.start();
newarray = benchmark.copyIntArray( array );
Loco.console.println( timer.elapsed );
timer.stop();

timer.start();
newarray = benchmark.copy( array );
Loco.console.println( timer.elapsed );
timer.stop();

function catchSignal( array ) {
  array[ 0 ] = array[ 2 ];
}
benchmark.emittedSignal.connect( catchSignal );
timer.start();
benchmark.emitSignal( SIZE, 100 );
Loco.console.println( timer.elapsed / 100 );
timer.stop();

Loco.ctx.exit( 0 );
} catch( e ) {
  Loco.console.printerrln( e );
  Loco.ctx.exit( -1 );
}


