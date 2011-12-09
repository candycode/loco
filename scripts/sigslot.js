try {

function assert( condition ) { if( !condition ) throw condition + ": not met"; }


if( Loco.ctx.commandLine.length < 3 ) {
  Loco.ctx.println( "Usage: " + Loco.ctx.commandLine[ 0 ] + " " +
                    Loco.ctx.commandLine[ 1 ] + "<plugin path>" );
  Loco.ctx.exit( 0 );
}

var ppath = Loco.ctx.commandLine[ 2 ] + "/";
var sourceName = "source.qtplugin";
var targetName = "target.qtplugin";
var source = Loco.ctx.loadQtPlugin( ppath + sourceName );
var target = Loco.ctx.loadQtPlugin( ppath + targetName );

// Note: you cannot call the aSignal/aSlot directly because the
//       std::string paramter type is not recognized as a valid
//       data type
source.link( "aSignal(std::string)", target, "aSlot(std::string)" );
source.emitSignal( "hello" );

Loco.ctx.exit( 0 );

} catch( e ) {
  Loco.console.printerrln( e );
  Loco.ctx.exit( -1 );
}



