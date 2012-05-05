#!/usr/bin/env locoplay
// Substitute tabs with spaces
try {
if( Loco.ctx.args.length < 3 ) {
  Loco.console.println( "usage: " + Loco.ctx.args[ 1 ] +
                        "<num spaces for tab> ['extension1, extension2...']" );
  Loco.console.exit( 1 );
}
function subst( file, spaces ) {
  Loco.console.println(file);
  var f = Loco.fs.fopen( file, ["r","w"] );
  if(f==null) return -1;
  var content = f.readAll();
  var prevContentLength = content.length;
  content = content.replace( /\t/g, spaces );
  f.reset();
  f.write( content );
  f.close();
}
var extensions = ["*"];
if( Loco.ctx.args.length > 3 ) extensions = Loco.args.split(","); 
var files = Loco.fs.dir().entries( extensions );
var NUMSPACES=Loco.ctx.args[ 2 ];
if( NUMSPACES < 1 ) throw( "wrong number of spaces (" + NUMSPACES + ")" );
var spaces = "";
for( var s=0; s != NUMSPACES; ++s ) spaces += " ";
for( var f in files ) {
  subst( f, spaces );
}  
Loco.ctx.exit( 0 );
} catch( e ) {
  Loco.console.printerrln( e );
  Loco.ctx.exit( -1 );
}
