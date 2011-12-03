
var c = Loco.console;
var fs = Loco.fs;
try {
c.println( "LoCO started..." );

(function() {
  var file = fs.fopen( "loco.txt", ["w"] );
  file.write( "Loco!!!" );
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
Loco.ctx.exit( 0 );
} catch( e ) {
  c.println( e );
  Loco.ctx.exit( 0 );	
}

  	
