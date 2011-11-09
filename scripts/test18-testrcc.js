var print = Loco.console.println;
var exit = Loco.ctx.exit;
try {
  if( !Loco.ctx.registerResources( "test18-qrc/test18.rcc" ) ) throw "resource registration failed";
  //by default extracted file is read-only; change permission to delte files between subsequent runs
  if( !Loco.fs.copy( ":/fileinresource.txt", "./test18-qrc/filefromresource.txt" ) ) throw "resource extraction failed"; 
  var fileContent = Loco.ctx.read( "./test18-qrc/filefromresource.txt" );
  if( !fileContent ) throw "file reading failed";
  print( fileContent );
  exit( 0 );
} catch( e ) { print( e ); exit( -1 ); }
