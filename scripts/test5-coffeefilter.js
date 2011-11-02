try {
  Loco.console.println("Loco started");
  Loco.console.println(Loco.ctx.jsInterpreterName() );
  if( Loco.ctx.jsInterpreterName().indexOf( "webkit" ) < 0 ) {
    Loco.ctx.include( "../filters/coffee/coffee-script-no-this.js" );
  } else {
    Loco.ctx.include( "../filters/coffee/coffee-script.js" );
  }
  var c = CoffeeScript.compile( "x = 32", {bare: true} );
  Loco.console.println( c );
  //Loco.console.println( CofeeScript.compile( "square = (x) -> x*x", {bare: on} ) ); 
  Loco.ctx.addScriptFilter( "coffeescript", "coffeeCompile" );
  var coffeeCompile = function( coffeeCode ) { 
    return CoffeeScript.compile( coffeeCode, {bare: true} );
  }
  var fromCoffee = function( s ) { Loco.console.println( s ); }
  Loco.ctx.evalFile( "./test5.coffee", ["coffeescript"] );
} catch(e) {
  Loco.console.printerrln( e );
}
