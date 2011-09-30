var c = Loco.console;
var sys = Loco.sys;
try {
  var p = sys.process();
  p.setProcessChannelMode( "merged" ); 

  c.println( "Invoking python -c " );
  p.start( "python", ["-c", "print 1+2"], [ 'r', 'text' ] );
  if( !p.waitForFinished() ) throw "Cannot start process";
  c.println( parseInt( p.readAll() ) + 6 ); 
  p.close();

  c.println();
  c.println( "Invoking interactvei interpreter python -i" );
  p.start( "python", ["-i"], ["w","r", "a"] );
  while( p.waitForReadyRead( 200 ) )
    /*c.println(*/ p.readAll(); // ); // eat header
  p.write( "1+2\n" );
  p.waitForBytesWritten( 300 );
  while( p.waitForReadyRead( 300 ) ) {
    var v = p.readLine();
    if( parseInt( v ) !== null ) c.println( parseInt( v ) + 6 );
  } 

  p.close(); 

} catch( e ) {
  c.printerrln( e );	
}

  	
