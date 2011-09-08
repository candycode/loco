
var c = Loco.console;
var fs = Loco.fs;
try {
c.println( "Started loco..." );
//c.println( fs.home() );
var file = fs.fopen( "loco.txt", ["w"] );
file.write( "Loco!!!");
file.close();   
} catch( e ) {
  c.println( e );	
}

  	
