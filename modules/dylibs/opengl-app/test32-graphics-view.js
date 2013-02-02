try {
var print = Loco.console.println;
var perror = Loco.console.printerrln;
var view = Loco.gui.create( "GraphicsView" );
//remove border
view.setStyleSheet( "QGraphicsView { border-style: none; }" );
Loco.ctx.suppressQtMessages( true );
Loco.ctx.onError.connect( function( msg ) { throw msg; } );
print( "Created graphics scene proxy to forward events" );
var openGLApp = Loco.ctx.loadQObjectFromDyLib( Loco.ctx.args[2], 'Create' );
if( openGLApp.error ) openGLApp.error.connect( function( msg ) { throw msg; } );
else if( openGLApp.onError ) openGLApp.onError.connect( function( msg ) { throw msg; } ); 
if( !openGLApp ) throw "Cannot load plugin";
var glformat = openGLApp.glFormat ? openGLApp.glFormat : { alpha: true, depth: true };
if( openGLApp.description ) Loco.gui.infoDialog( "Info", openGLApp.description ); 
view.createOpenGLViewport( glformat );
view.setViewportUpdateMode( "full" );
print( "Configured OpenGL viewport" );
print( "Created graphics view" );
print( "Loaded OpenGL app" );
print( "Checking if init() slot exists..." );
if( openGLApp.init ) {
  openGLApp.init( Loco.ctx.args.slice( 2, Loco.ctx.args.length - 1 ) );
  print( "...init() found and invoked" );
}
view.createGraphicsSceneProxy();
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
print( "Checking for OpenGL init slot..." );
if( openGLApp.initGL ) {
  openGLApp.initGL();
  print( "...'initGL' found and invoked" );
} else if( openGLApp.initgl ) {
  openGLApp.initgl();  
  print( "...'initgl' found and invoked" );
} else print( "...no slot found" );
print( "Checking if 'resize' or 'resized' slots exist..." );
if( openGLApp.resize ) {
  view.resized.connect( openGLApp.resize );
  print( "...'resize' found and connected to GraphicsView.resized signal" ); 
} else if( openGLApp.resized ) {
  view.resized.connect( openGLApp.resized );
  print( "...'resized' found and connected to GraphicsView.resized signal" );
} else print( "...no slot found" ); 
if( openGLApp.name ) view.setWindowTitle( openGLApp.name );
view.show( 50, 50, 800,600 );
} catch( e ) {
if( e.message ) perror( p.message )
else perror( e );
Loco.ctx.exit( -1 );
} 

