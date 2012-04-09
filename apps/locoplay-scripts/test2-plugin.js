try {
  var BUILD_DIR = Loco.fs.home() + '/build/loco';
  var PLUGIN_NAME = "libloco_SamplePlugin.so"; 
  if( Loco.ctx.os() === "WINDOWS" ) {
    PLUGIN_NAME = "loco_SamplePlugin.dll";  
  }
  var PLUGIN_PATH = BUILD_DIR + '/modules/plugins/sample/';
  var plugin = Loco.ctx.loadObject( PLUGIN_PATH + PLUGIN_NAME );
  plugin.hello();
  Loco.ctx.exit( 0 );
} catch( e ) {
  Loco.console.println( e );
  Loco.console.pritnln( "Make sure the sample plugin is built and the path is correct" );
  Loco.ctx.exit( -1 );   
}


