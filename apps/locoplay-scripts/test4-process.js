var c = Loco.console;
var sys = Loco.sys;
try {
  var p = sys.process();
  p.setProcessChannelMode( "merged" );
  if( Loco.sys.os() === "WINDOWS" ) {
    p.start( "cmd.exe", [], ['r','w'] );
    //p.start( "cmd.exe", ["/C", "dir", "C:"], ['r','w'] );
    if( !p.waitForStarted() ) throw "Cannot start process";
    p.write( "dir C:\n" );
    p.waitForReadyRead();
    c.println( p.readAll() );
  } else {
    p.start( "ls", [], ["r"] );
    if( !p.waitForFinished() ) throw "Cannot start process";
    c.println( p.readAll() );
  }
  p.close();
  Loco.ctx.exit( 0 );
} catch( e ) {
  c.printerrln( e );
  Loco.ctx.exit( -1 );	
}

  	
