try {
var ppath = "";
if( Loco.ctx.os() === "WINDOWS" ) 
  ppath = "/cmakebuilds/project-loco/modules/plugins/osgview/Release/osgview.dll";
else
  ppath = Loco.ctx.homeDir() + "/build/loco/modules/plugins/osgview/libosgview.so";
var osgview = Loco.ctx.loadQtPlugin( ppath );
if( !osgview ) throw "Cannot create OSG view";
var mw = Loco.gui.create( "Widget");
var l = Loco.gui.create( "HBoxLayout" );
l.addWidget( osgview );
l.setContentsMargins( 0, 0, 0, 0 );
mw.setLayout( l );
osgview.loadPage( "./overlay.html", true );
osgview.setContinuousUpdate( 20 );
osgview.loadScene( "cow.osg" );
mw.show(800, 600);
} catch(e) {
Loco.console.println(e);
Loco.ctx.exit( -1 );
}
