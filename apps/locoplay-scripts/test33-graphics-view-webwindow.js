try {
var print = Loco.console.println;
var perror = Loco.console.printerrln;
if( Loco.ctx.args.length < 3 ) {
  print( "Usage: locoplay " + Loco.ctx.args[ 1 ] + " <plugin path>" );
  Loco.ctx.exit( 1 );
}
var glapp = Loco.ctx.loadQtPlugin( Loco.ctx.args[ 2 ] );
if( !glapp ) throw "Couldn't load plugin " + Loco.ctx.args[ 2 ];
var glformat = glapp.glFormat ? glapp.glFormat : { alpha: true, depth: true };
if( glapp.description ) Loco.gui.infoDialog( "Info", glapp.description ); 
if( glapp.init ) glapp.init({});
var view = Loco.gui.create( "GraphicsView" );
//remove border
view.setStyleSheet( "QGraphicsView { border-style: none; }" );
if( glapp.name ) view.setWindowTitle( glapp.name );
Loco.ctx.suppressQtMessages( true );
Loco.ctx.onError.connect( function( msg ) { throw msg; } );
var glformat =  { alpha: true, depth: true };
if( glapp.initGL ) view.initGL.connect( glapp.initGL );
view.createOpenGLViewport( glformat );
view.setViewportUpdateMode( "full" );
view.createGraphicsSceneProxy();
var ww = Loco.gui.create( "GraphicsWebWindow" );
view.scene.add( ww );
view.scene.backDraw.connect( glapp.render );
view.resized.connect( ww.resize );
if( glapp.resize ) view.resized.connect( glapp.resize );
else if( glapp.resized ) view.resized.connect( glapp.resized ); 
// make application accessible from html page
ww.addObjectToContext( glapp, "app" );
ww.setTransparent();
//async version
//ww.loadFinished.connect( function( ok ) { 
//                           if( !ok ) throw "Cannot load page";
//                           view.show( 50, 50, 800,600 );
//                         } );
//if( glapp.guiURL ) ww.load( glapp.guiURL );
//else ww.load( "./test33-overlay.html" );
if( glapp.guiURL ) ww.syncLoad( glapp.guiURL );
else ww.syncLoad( "./test33-overlay.html" );
view.show( 2000, 50, 800, 600 );
} catch( e ) {
if( e.message ) perror( p.message )
else perror( e );
Loco.ctx.exit( -1 );
} 

