function execute( path ) {
  var prefix = ":/" + path + "/";
  if( Loco.fs.fexists( prefix + "main.js" ) ) {
    Loco.ctx.evalFile(  prefix + "main.js" );
  } else if( Loco.fs.fexists( prefix + "main.coffee" ) ) {
    Loco.ctx.evalFile( prefix + "main.coffee" );
  } else if( Loco.fs.fexists( prefix + "main.py" ) ) {
    Loco.ctx.evalFile( prefix + "main.py" );
  }
}

var rx = /([^/]+)(?:\.rcc$|\.lrcc$)/i;
var path;
for( var i = 0; i != ctx.cmdLine().length; ++i ) {
    Loco.ctx.registerResources( ctx.cmdLine()[ i ] );
    path = rx.exec( ctx.cmdLine()[ i ] );
    if( path && path.length > 1 ) {
      execute( path[ 1 ] );  
    }
}
  