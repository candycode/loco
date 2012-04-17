try {
var p = Loco.console.println;
var mw = Loco.gui.create( "MainWindow" );
var osgWidgetPath;
if( Loco.ctx.os() === "WINDOWS" )
  osgWidgetPath="/cmakebuilds/loco/modules/webplugins/gl/osgglviewer.dll";
else
  osgWidgetPath=Loco.fs.home() + "/build/loco/modules/webplugins/gl/libosgglviewer.so"
var osgWin = Loco.ctx.loadQtPlugin( osgWidgetPath );
if( !osgWin ) throw "Cannot load plugin";
var ww = Loco.gui.create( "WidgetWrapper" );
var webWindow = Loco.gui.create( "WebWindow" );
ww.setWidget( osgWin );
mw.setCentralWidget( ww );
osgWin.loadScene( "cow.osg" );
mw.show();
} catch(e) {
Loco.console.println(e);
}
