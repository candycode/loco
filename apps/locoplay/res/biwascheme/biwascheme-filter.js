var BiwaScheme = BiwaScheme || {};
var Console = {};
if( Loco.ctx.jsInterpreterName().indexOf( "webkit" ) >= 0 ) {
  Loco.ctx.include( ":/biwascheme/biwascheme_closure_compiled.js" );
  Console.puts = function(str, no_newline) {
    Loco.console.print(str + (no_newline ? "\n" : "") );
  };
  Console.p = function() {
    Loco.console.print.apply(this, arguments);
  };
} else {
  Loco.ctx.include( ":/biwascheme/biwascheme_no_web_closure_compiled.js" );
  Console.puts = function(str, no_newline) {
    alert(str + (no_newline ? "\n" : "") );
  };
  Console.p = function() {
    alert.apply(this, arguments);
  };	
}
function locoHandleBiwaSchemeError_( e ) { 
  if( e.message ) throw e.message;
  else throw e;
}