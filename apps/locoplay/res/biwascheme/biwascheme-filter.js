var BiwaScheme = BiwaScheme || {};
function locoHandleBiwaSchemeError_( e ) { 
  if( e.message ) throw e.message;
  else throw e;
}
var Console = {};
Loco.ctx.include( ":/biwascheme/biwascheme-min.js" );
Console.puts = function(str, no_newline) {
  Loco.console.print(str + (no_newline ? "\n" : "") );
};
Console.p = function() {
  Loco.console.print.apply(this, arguments);
};

BiwaScheme.define_libfunc("print", 1, 1, function(args) {
   Loco.console.println( args[ 0 ] );
   return true;
});
BiwaScheme.define_libfunc("open-web-page", 1, 1, function(args) {
   var ww = Loco.gui.create( "WebWindow" );
   ww.load( args[ 0 ] );
   ww.show();
   return true;
});
BiwaScheme.define_libfunc("prompt", 1, 1, function(args) {
   return prompt( args[ 0 ] );
});
BiwaScheme.define_libfunc("include", 1, 1, function(args) {
   Loco.ctx.include( args[ 0 ] );
   return true;
});
