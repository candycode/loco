

var res = 0.0;
var count = 0;
var elapsed = 0;
var  t = 0;
var f = function() {
  res += Math.log( Math.sqrt( Math.sin( res * Math.PI + Math.PI / 4 ) * Math.cos( res * Math.PI + Math.PI / 4 ) ) );
}
t = ( new Date() ).getTime();
var start = Loco.sys.clock();
while( ( ( new Date() ).getTime() - t ) <  1000 ) {
  f();
  //elapsed = Loco.sys.clock() - start;
  ++count;  	
}
Loco.console.println( count  );
Loco.ctx.exit( 0 );


