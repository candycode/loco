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
//osgview.loadScene( "cow.osg" );
//osgview.loadPage( "file:///projects/loco/modules/plugins/osgview/test/loadtest.html" );
//osgview.loadPage( "file:///projects/loco/modules/plugins/osgview/test/loadtest.html", true );
//mw.setCentralWidget( osgview );
osgview.loadPage( "./loadtest.html", true );
osgview.setContinuousUpdate( 20 );
////osgview.setOpacity(  );
osgview.loadScene( "cow.osg" );
mw.show();
} catch(e) {

Loco.console.println(e);

}
