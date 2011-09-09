
var c = Loco.console;
var fs = Loco.fs;
try {
c.println( "Started loco..." );
//c.println( fs.home() );

(function() {
  var file = fs.fopen( "loco.txt", ["w"] );
  file.write( "Loco!!!");
  file.close();
})();   
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


} catch( e ) {
  c.println( e );	
}

  	
