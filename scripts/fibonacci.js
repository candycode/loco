// The required (regardless of the project scope) Fibonacci functions

var createTimer = function() {
                   return ( function() {
                    var start_, stop_; 
                    var t = { start: function() { start_ = function() { return ( new Date() ).getTime(); } },
                              stop:  function() { end_   = function() { return ( new Date() ).getTime(); } },
                              elapsed: function() { return end_() - start_(); },
                              getStart: function() { return start_(); } };
                    return t; } ) (); 
                  }

// iterative
function ifib_( n ) {
  var f_minus_2 = 0; 
  var f_minus_1 = 1;
  var f;
  for( var i = 2; i <= n; ++i ) {
    f = f_minus_2 + f_minus_1;
    f_minus_2 = f_minus_1;
    f_minus_1 = f;
  }
  return f; 
}

// recursive
function rfib_( n ) {
  if( n === 0 || n === 1 ) return 1;
  return rfib_( n - 2 ) + rfib_( n - 1 ); 
}

// recursive w/ memoization
function mfib_( n ) {
  var cache = [0,1,1];
  return ( function fib( n ) {
    if( cache[ n ] === undefined ) cache[ n ] = fib( n - 2 ) + fib( n - 1 );
    return cache[ n ];
  })( n ); 
}

var fib = ifib_;

var timer  = createTimer();
timer.start();
var f = fib( 50 );
timer.stop();
print( timer.elapsed() );
print( f );

Loco.ctx.exit( 0 );


















