try {

var ww = Loco.gui.create( "WebWindow" );
ww.createWebPluginFactory();
ww.load( "./loadtest.html" );
ww.show();
Loco.sys.exec();

} catch(e) {

Loco.console.println(e);

}
