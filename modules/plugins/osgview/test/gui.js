try {
var ppath = "";
Loco.console.println(Loco.ctx.args[2])
var osgview = Loco.ctx.loadQtPlugin( Loco.ctx.args[2] );
if( !osgview ) throw "Cannot create OSG view";
Loco.ctx.suppressQtMessages( true );
var mw = Loco.gui.create( "Widget");
mw.setWindowTitle( "osgviewer" );
var l = Loco.gui.create( "HBoxLayout" );
l.addWidget( osgview );
l.setContentsMargins( 0, 0, 0, 0 );
mw.setLayout( l );
osgview.setContinuousUpdate( 20 );
osgview.addJSObject( "gui", Loco.gui );
osgview.addJSObject( "win", mw );
osgview.addJSObject( "ctx", Loco.ctx );
osgview.jsinit = "eval(ctx.read('jquery.js')); \
                  eval(ctx.read('jquery-ui.js')); \
                  eval(ctx.read('knockout.js'));";
osgview.loadPage( "./gui.html", true );
if( Loco.ctx.args.length > 2 ) {
  osgview.loadScene( Loco.ctx.args[ 2 ] );
} else osgview.loadScene( "dumptruck.osg" );
mw.show(800, 600);
} catch(e) {
Loco.console.println(e);
Loco.ctx.exit( -1 );
}
