// Native array test

var p = Loco.console.println;


try {
var timer = Loco.sys.timer();
var SIZE = 10000;
var i = 0;
var jsArray = new Array( SIZE );
// double precision array
var cppArray = Loco.ctx.create( "Float64Array", {align:8,size:SIZE} );
p();

p( "Initializing javascript array with for loop" );
timer.start();
for( i = 0; i != SIZE; ++i ) jsArray[ i ] = i;
timer.stop();
Loco.console.println( "Elapsed time: " + timer.elapsed.toFixed( 2 ) + " ms" );
p();


p( "Initializing native array with for loop" );
//this should be 100x slower 
//cppArray.resize( SIZE );
timer.start();
for( i = 0; i != SIZE; ++i ) cppArray.set( i, i );
timer.stop();
Loco.console.println( "Elapsed time: " + timer.elapsed.toFixed( 2 ) + " ms" );
p();

p( "Initializing native array with javascript array" );
//much faster than above
timer.start();
cppArray.mset( 0, jsArray );
timer.stop();
Loco.console.println( "Elapsed time: " + timer.elapsed.toFixed( 2 ) + " ms" );

Loco.ctx.exit( 0 );
} catch( e ) {
  Loco.console.printerrln( e );
  Loco.ctx.exit( -1 );
}


