
var c = Loco.console;
var fs = Loco.fs;
try {
(function() {
  var file = fs.fopen( "loco.txt", ["w"] );
  file.write( "LoooCO!!!" );
  file.close();
  c.println( "Written to file loco.txt" );
})();   
(function() {
  var file = fs.fopen( "loco.txt", ["r"] );
  c.println( "Read from loco.txt: " + file.readLine() );
  file.close();
})();   

c.println( "press <ENTER> to continue" );
c.readLine();

(function() {
   var dir = fs.dir( "/" );
   var entries = dir.entries();
   for( e in entries ) {
     c.println( e + ': ' );
     for( v in entries[ e ] ) {
     	 if( v === 'absolute_path' ) continue; 
     	 c.print( '\t' + v + ': ' );
     	 c.print( entries[ e ][ v ] );
     	 c.println();
     }
     c.println(); 
   } 	
})();
Loco.ctx.exit( 0 );
} catch( e ) {
  c.println( e );
  Loco.ctx.exit( 0 );	
}

  	
