try {
Loco.ctx.onError.connect( function( err ) { throw err; } );
var ppath = "";
if( Loco.ctx.os() === "WINDOWS" ) 
  ppath = "/cmakebuilds/project-loco/modules/plugins/glviewer/Release/glviewer.dll";
else
  ppath = Loco.ctx.homeDir() + "/build/loco/modules/plugins/glviewer/locoviewer.qtplugin";
Loco.console.println( ppath );
var glviewer = Loco.ctx.loadQtPlugin( ppath );
if( !glviewer ) throw "Cannot create OpenGL viewer";
var mw = Loco.gui.create( "MainWindow" );
mw.setCentralWidget( glviewer );
glviewer.load( "models/qt.obj" );
mw.show();

} catch(e) {
Loco.console.println(e);
Loco.ctx.exit( -1 );
}
