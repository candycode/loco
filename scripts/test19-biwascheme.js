try {

function objinfo( obj ) {
  Loco.console.println("===========================");
  for( var i in obj ) {
    /*if( obj.hasOwnProperty( i ) )*/ {
      Loco.console.println( i + ": " + obj[ i ] );
    }
  }
}

var print = Loco.console.println;
Loco.ctx.include( "../filters/biwascheme/biwascheme-min.js" );
objinfo( BiwaScheme ) 
var compiler = new BiwaScheme.Compiler();
var compiled = compiler.compile('(+ 1 2)');
objinfo( compiled );
var interpreter = new BiwaScheme.Interpreter();
objinfo( interpreter );
var interpreted = interpreter.evaluate( "(+ 1 2)" );
objinfo( interpreted );
Loco.ctx.exit( 0 );
} catch(e) { Loco.console.println(e); }
