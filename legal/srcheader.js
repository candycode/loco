#!/usr/bin/env locoplay
// Substitute placeholder with text read from file
try {
if( Loco.ctx.args.length < 4 ) {
  Loco.console.println( "usage: " + Loco.ctx.args[ 1 ] +
                        " <placeholder> <text>" );
  Loco.console.exit( 1 );
}
var p = Loco.console.println;
p( Loco.ctx.args );
function subst( file, placeholder, text ) {
  p(file);
  var f = Loco.fs.fopen( file, ["r","w"] );
  var content = f.readAll().replace( placeholder, text );
  f.reset();
  f.write( content );
  f.close();
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
