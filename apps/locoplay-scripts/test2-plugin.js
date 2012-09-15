try {
  var PLUGIN_PATH = ""
  var PLUGIN_NAME = "loco_SamplePlugin.qtplugin"; 
  if( Loco.ctx.args.length < 3 ) {
    var BUILD_DIR = Loco.fs.home() + '/build/loco';
    PLUGIN_PATH = BUILD_DIR + '/modules/plugins/sample';
  } else {
    PLUGIN_PATH = Loco.ctx.args[ 2 ];
  }
  var plugin = Loco.ctx.loadObject( PLUGIN_PATH + '/' + PLUGIN_NAME );
  if( plugin === null ) throw "Cannot load plugin " + PLUGIN_PATH + '/' + PLUGIN_NAME;
  plugin.hello();
  Loco.ctx.exit( 0 );
} catch( e ) {
  Loco.console.println( e );
  Loco.console.println( "Make sure the sample plugin is built and the path is correct" );
  Loco.ctx.exit( -1 );   
}


