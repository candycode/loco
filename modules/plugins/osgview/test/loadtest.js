try {

var ppath = "";
if( Loco.ctx.os() === "WINDOWS" ) 
  ppath = "/cmakebuilds/project-loco/modules/plugins/osgview/Release/osgview.dll";
else
  ppath = Loco.ctx.homeDir() + "/build/loco/modules/plugins/osgview/libosgview.so";

var osgview = Loco.ctx.loadObject( ppath );
osgview.loadScene( "cow.osg" );
//osgview.loadPage( "file:///projects/loco/modules/plugins/osgview/test/loadtest.html" );
//osgview.loadPage( "file:///projects/loco/modules/plugins/osgview/test/loadtest.html", true );
//osgview.loadPage( "./loadtest.html", true );
osgview.loadPage( "http://airtightinteractive.com/demos/js/ruttetra/" );
//osgview.setOpacity(  );
osgview.show( 1024, 768 );

} catch(e) {

Loco.console.println(e);

}
