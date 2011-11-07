Loco.ctx.include( ":/skulpt/skulpt.js" );
Loco.ctx.include( ":/skulpt/builtin.js" );

var locoprint_;

if( Loco.ctx.jsInterpreterName().indexOf( "webkit" ) >= 0 ) {
  locoprint_ = function( msg ) {
    alert( msg );
  }
} else {
  locoprint_ = function( msg ) {
    Loco.console.print( msg );
  }
}

Sk.configure( {output: locoprint_, read: Loco.ctx.read} );
if( Loco.ctx.jsInterpreterName().indexOf( "webkit" ) >= 0 ) {
  Sk.builtin.openWebWindow = function( s ) {
      var WW = Loco.gui.create( "WebWindow" );
      WW.load( s.v );
      WW.show();
  }
  Sk.builtins[ "openWebWindow" ] = Sk.builtin.openWebWindow;
}
