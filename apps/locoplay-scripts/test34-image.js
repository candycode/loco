var print = Loco.console.println;
var perror = Loco.console.printerrln;
try {
var img = Loco.graphics.create( "Image2D" );
if( !img ) throw "Error";
if( !img.load( "test34-image/pic.jpg" ) ) throw "Cannot load image";
print( "Loaded image" );
print( "Image width: " + img.width + ", Image height: " + img.height );
var ww = Loco.gui.create( "WebWindow" );
ww.addObjectToContext( img, "image" ); 
ww.load( "test34-image/gui.html" );
ww.show( img.width, img.height );
} catch( e ) {
  if( e.message ) Loco.console.printerrln( e.message );
  else Loco.console.printerrln( e );
  exit( -1 ); // will generate WaitCondition error message if message loop active
}



