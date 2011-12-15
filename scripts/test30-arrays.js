// copy 1M array to c++ and back from c++: 165 ms
// invoking a js signal from a c++ method and passing 1M elements: 170 ms
// 10k elements: 1.6 ms copy, 1.6 ms invocation 

try {
var timer = Loco.sys.timer();
var SIZE = 10000;
var i = 0;
var jsArray = new Array( SIZE );
var cppArray = Loco.ctx.create( "Float64Array", {align:8,size:SIZE} );
Loco.console.println( cppArray );

timer.start();
for( i = 0; i != SIZE; ++i ) jsArray[ i ] = i;
timer.stop();
Loco.console.println( timer.elapsed );
timer.stop();

//this should be 100x slower 
//cppArray.resize( SIZE );
timer.start();
for( i = 0; i != SIZE; ++i ) cppArray.set( i, i );
timer.stop();
Loco.console.println( timer.elapsed );
timer.stop();
Loco.console.println( cppArray.at( 24 ) );

//much faster than above
timer.start();
cppArray.mset( 0, jsArray );
timer.stop();
Loco.console.println( timer.elapsed );

Loco.ctx.exit( 0 );
} catch( e ) {
  Loco.console.printerrln( e );
  Loco.ctx.exit( -1 );
}


