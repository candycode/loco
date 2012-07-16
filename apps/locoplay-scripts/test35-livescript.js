
try {
// livescript prelude adds data into global 'exports' variable
var exports = {};
Loco.ctx.include( "../../filters/livescript/prelude.js" );
Loco.ctx.include( "../../filters/livescript/livescript.js" );
Loco.ctx.eval( LiveScript.compile( Loco.fs.fread( "test35.ls" ) ) );
Loco.ctx.exit( 0 );
} catch( e ) {
  if( e.message ) Loco.console.printerrln( e.message );
  else Loco.console.printerrln( e );
  Loco.ctx.exit( -1 );
}





