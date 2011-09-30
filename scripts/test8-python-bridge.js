try {

   
  var c = Loco.console;
  var sys = Loco.sys;
  var p = sys.process();
  
  c.println( "\nLoCO's Python wrapper, type 'q.<enter>' to exit" );

  p.setProcessChannelMode( "merged" ); 
  p.start( "python", ["-V"], ["r"] );
  p.waitForFinished();
  c.println( p.readAll() );
  p.close();

  var prompt = "pyLoco>";
  var indent = "      ";
  c.print(prompt);
  input = c.readLine() + "\n";
  p.start( "python", ["-i"], ["w","r"] );
  while( p.waitForReadyRead( 100 ) )
    p.readAll()
  

  var pyAppend = function( p, code ) {
    p.write( code + "\n" ); 
    p.waitForBytesWritten();
    while( p.waitForReadyRead( 100 ) ) 
      p.readAll();  
  };


  pyAppend( p, "history=[]" );
  
  while( input !== "q.\n" ) {
    p.write( input );
    if( !p.waitForBytesWritten( 100 ) ) throw "Write error";
    while( p.waitForReadyRead( 100 ) ) {
      c.print( p.readAll()
                  .replace( ">>> ", prompt )
                  .replace( "...", indent ) );
    }
    input = c.readLine();
    pyAppend( p, "history.append(" + '"' + input + '"' + ")" );
    input = input.concat( "\n" );
  } 
  p.close(); 
} catch( e ) {
  c.printerrln( e );	
}

  	
