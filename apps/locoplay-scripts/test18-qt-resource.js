var print = Loco.console.println;
var exit = Loco.ctx.exit;
try {
  if( !Loco.ctx.registerResources( "test18-qrc/test18.rcc" ) ) throw "resource registration failed";
  //by default extracted file is read-only; change permission to delte files between subsequent runs
  if( !Loco.fs.copy( ":/fileinresource.txt", "./test18-qrc/filefromresource.txt" ) ) throw "resource extraction failed"; 
  print( "Extracted file 'fileinresource.txt' from resource bundle" );
  var fileContent = Loco.fs.readAll( "./test18-qrc/filefromresource.txt" );
  //QFile::readAll adds '\n' !!!
  fileContent =  fileContent.substring( 0, fileContent.length - 1 );
  if( !fileContent ) throw "file reading failed";
  print( "Reading file..." );
  print( "'" + fileContent + "'" );
  print( "...removing file" );
  Loco.fs.remove( "./test18-qrc/filefromresource.txt" );
  exit( 0 );

  for(i in Loco.fs ) print( i );
} catch( e ) { print( e ); exit( -1 ); }
