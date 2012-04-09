// In the order of 840k loop executions on a 2008 Penryn Xeon 5420 processor, 
// Ubuntu 11.10 64bit, Qt 4.8 RC;
// assuming the number of flops executed within the loop is around 100
// it means around 84 MFlops double precision performance
// Allocation and copy of a 1M element array is around 7.8 ms!

try {
var timer = Loco.sys.timer();
var counter = 0;
var res = 0.0;
var f = function() {
  res += Math.log( Math.sqrt( 
           Math.sin( Math.PI * res + Math.PI / 4 ) / 
           Math.exp( Math.cos( Math.PI * res + Math.PI / 4 ) ) ) );
}
timer.start();
while( timer.elapsed < 1000 ) {
  f();
  ++counter;
}
timer.stop();
Loco.console.println( counter );

var SIZE = 1000000;
var array = new Array( SIZE );
timer.start();
for( var i = 0; i != SIZE; ++i ) {
  array[ i ] = i;
}
timer.stop();
Loco.console.println( timer.elapsed.toFixed( 2 ) );

timer.start();
var arrayCopy = array.slice( 0 );
timer.stop();
Loco.console.println( timer.elapsed.toFixed( 2 ) );

timer.start();
var arrayCopy2 = new Array( SIZE );
for( var i = 0; i != SIZE; ++i ) {
  arrayCopy2 = array[ i ];
}
timer.stop();
Loco.console.println( timer.elapsed.toFixed( 2 ) );

Loco.ctx.exit( 0 );
} catch( e ) {
  Loco.console.printerrln( e );
  Loco.ctx.exit( -1 );
}


