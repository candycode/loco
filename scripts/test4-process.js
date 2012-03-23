var c = Loco.console;
var sys = Loco.sys;
try {
  var p = sys.process();
//  p.setStandardOutputFile( "out.txt", ["w"] );
  
  if( Loco.sys.os() === "WINDOWS" ) {
    p.start( "cmd.exe", [""], ['r','w'] );
    //p.start( "cmd.exe", ["/C", "dir", "C:"], ['r','w'] );
    if( !p.waitForStarted() ) throw "Cannot start process";
    //if( !p.waitForReadyRead( 10000 ) ) throw "Cannot read from process";
    var res = p.write( "dir C:\n" );
  } else {
    p.start( "ls", [""], ["r","w"] );
    if( !p.waitForStarted() ) throw "Cannot start process";
  }
  p.close();
  Loco.ctx.exit( 0 );
} catch( e ) {
  c.printerrln( e );
  Loco.ctx.exit( -1 );	
}

  	
