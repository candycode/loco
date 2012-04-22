
//issues on Ubuntu 11.10 64bit, NVIDIA drivers v 280.x qt 4.8.1:
//1) cannot use single widget, need to make QGraphicsView child
//   of another widget
//2) warnings: 
//     QGLShader: could not create shader 
//     Warning: "" failed to compile! 
//     QGLContext::makeCurrent(): Cannot make invalid context current.

try {
Loco.ctx.onError.connect( function( err ) { throw err; } );
var ppath = "";
if( Loco.ctx.os() === "WINDOWS" ) 
  ppath = "/cmakebuilds/project-loco/modules/plugins/glviewer/Release/glviewer.dll";
else
  ppath = Loco.ctx.homeDir() + "/build/loco/modules/plugins/glviewer/glviewer.qtplugin";
Loco.console.println( ppath );
var glviewer = Loco.ctx.loadQtPlugin( ppath );
if( !glviewer ) throw "Cannot create OpenGL viewer";
var main = Loco.gui.create( "Widget" );
var layout = Loco.gui.create( "HBoxLayout" );
layout.addWidget( glviewer );
var inlayout = Loco.gui.create( "HBoxLayout" );
var w = Loco.gui.create( "WebWindow" );
var ww = Loco.gui.create( "WidgetWrapper" );
w.setTransparent();
ww.wrap( glviewer );
inlayout.addWidget( w );
ww.setLayout( inlayout );
main.setLayout( layout);
if( Loco.ctx.args.length === 3 
    && Loco.fs.exists( Loco.ctx.args[ 2 ] ) ) {
  glviewer.load( Loco.ctx.args[ 2 ] );
} else glviewer.load( "models/qt.obj" );
w.load( "./loadtest.html" );
main.show();
} catch(e) {
Loco.console.println(e);
Loco.ctx.exit( -1 );
}
