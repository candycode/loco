var c = Loco.console;
var sys = Loco.sys;
try {
  var p = sys.process();
  p.setStandardOutputFile( "out.txt", ["w"] );
  p.start( "cmd.exe", [""], ['r','w'] );
  //p.start( "cmd.exe", ["/C", "dir", "C:"], ['r','w'] );
  if( !p.waitForStarted() ) throw "Cannot start process";
  //if( !p.waitForReadyRead( 10000 ) ) throw "Cannot read from process";
  var res = p.write( "dir C:\n" );
  p.closeWriteChannel(); 
  if( !p.waitForBytesWritten( 2000 ) ) throw "Cannot write to process";   
  p.close();
} catch( e ) {
  c.println( e );	
}

  	
