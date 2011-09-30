Loco.console.println("Started");

try {
  var plugin = Loco.ctx.loadObject( "loco_TestPlugin" );
  plugin.hello();
} catch( e ) {
  Loco.console.println( e );   
}


