function execute( path ) {
  var prefix = ":/" + path + "/";
  if( Loco.fs.fexists( prefix + "main.js" ) ) {
    Loco.ctx.evalFile( prefix + "main.js" );
  } else if( Loco.fs.fexists( prefix + "main.coffee" ) ) {
    Loco.ctx.evalFile( prefix + "main.coffee" );
  } else if( Loco.fs.fexists( prefix + "main.py" ) ) {
    Loco.ctx.evalFile( prefix + "main.py" );
  }
}

var rx = /([^/]+)(?:\.rcc$|\.lrcc$)/i;
var path;
for( var i = 0; i != Loco.ctx.cmdLine().length; ++i ) {
    path = rx.exec( Loco.ctx.cmdLine()[ i ] );
    if( path && path.length > 1 ) {
      if( !Loco.ctx.registerResources( Loco.ctx.cmdLine()[ i ] ) ) {
        throw "Error registering resources " + Loco.ctx.cmdLine()[ i ];
      } 
      execute( path[ 1 ] );  
    }
}
  
