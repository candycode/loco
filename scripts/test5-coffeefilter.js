try {
  Loco.console.println("Interpreter: " + Loco.ctx.jsInterpreterName() );
  var WKIT = Loco.ctx.jsInterpreterName().indexOf( "webkit" ) >= 0;
  // global 'this' does not work properly in non-webkit interpreter
  if( !WKIT ) {
    Loco.ctx.include( "../filters/coffee/coffee-script-no-this.js" );
  } else {
    Loco.ctx.include( "../filters/coffee/coffee-script.js" );
  }
  var c = CoffeeScript.compile( "x = 32", {bare: true} );
  Loco.console.println( "COFFEE: x = 32\nJAVASCRIPT:\n" + c );
  Loco.ctx.addScriptFilter( "coffeescript", "coffeeCompile" );
  // if the interpreter is 'qtscript' the value returned by the eval() method
  // is not automatically converted to string
  var coffeeCompile = function( coffeeCode ) {
    if( typeof coffeeCode !== "string" ) coffeeCode = coffeeCode.toString();
    return CoffeeScript.compile( coffeeCode, {bare: true} );
  }
  //var fromCoffee = function( s ) { Loco.console.println( s ); }
  Loco.ctx.evalFile( "./test5.coffee", ["coffeescript"] );
  Loco.ctx.exit( 0 );
} catch( e ) {
  Loco.console.printerrln( e );
  Loco.ctx.exit( -1 );
}
