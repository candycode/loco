try {
var print = Loco.console.println;
var perror = Loco.console.printerrln;
var view = Loco.gui.create( "GraphicsView" );
print( "Created graphics view" );
view.createGraphicsSceneProxy();
print( "Created graphics scene proxy to forward events" );
var openGLApp = Loco.ctx.loadQtPlugin( Loco.ctx.args[ 2 ] );
if( !openGLApp ) throw "Cannot load plugin";
print( "Loaded OpenGL app" );
print( "Checking if init() slot exists..." );
if( openGLApp.init ) {
  openGLApp.init( Loco.ctx.args.slice( 2, Loco.ctx.args.length - 1 ) );
  print( "...init() found and invoked" );
}
print( "Checking for suitable render slot..." );
if( openGLApp.render ) {
  view.scene.backDraw.connect( openGLApp.render );
  print( "...'render()' found and connected to GraphicsScene.backDraw signal" );
} else if( openGLApp.draw ) {
  view.scene.backDraw.connect( openGLApp.draw );
  print( "...'draw()' found and connected to GraphicsScene.backDraw signal" );
} else if( openGLApp.paint ) {
  view.scene.backDraw.connect( openGLApp.paint );
  print( "...'paint()' found and connected to GraphicsScene.backDraw signal" );
} else throw "Could not find a suitable paint slot; tried 'render', 'draw' and 'paint'";
var glformat = openGLApp.glFormat ? openGLApp.glFormat : 
               { alpha: true,
                 depth: true,
                 profile: "compatibility",
                 rgba: true }; 
view.createOpenGLViewport( glformat );
print( "Configured OpenGL viewport" );
view.show();
} catch( e ) {
if( e.message ) perror( p.message )
else perror( e );
Loco.ctx.exit( -1 );
} 

