#!/usr/bin/env locoplay
// Substitute placeholder with text read from file
try {
if( Loco.ctx.args.length < 4 ) {
  Loco.console.println( "usage: " + Loco.ctx.args[ 1 ] +
                        " <placeholder> <text>" );
  Loco.ctx.exit( 1 );
}
var p = Loco.console.println;
p( Loco.ctx.args );
function subst( file, placeholder, text ) {
  var f = Loco.fs.fopen( file, ["r","w"] );
  var content = f.readAll();
  var substituted = false;
  if( content.indexOf( placeholder ) >= 0 ) {
    content = content.replace( placeholder, text );
    f.reset();
    f.write( content );
    substituted = true;
  }
  p( file + ": " + (substituted ? "yes" : "no") );
  f.close();
  return substituted;
}
var files = Loco.fs.dir().entries( ["*.cpp", "*.h", "*.js"] );
var PLACEHOLDER=Loco.ctx.args[ 2 ];
p( Loco.ctx.args[ 3 ] );
var TEXT=Loco.fs.fread( Loco.ctx.args[ 3 ] );
p(TEXT);
for( var f in files ) {
  subst( f, PLACEHOLDER, TEXT );
}  
Loco.ctx.exit( 0 );
} catch( e ) {
  Loco.console.printerrln( e );
  Loco.ctx.exit( -1 );
}
