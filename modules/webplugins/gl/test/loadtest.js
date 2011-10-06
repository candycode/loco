try {

var ww = Loco.gui.create( "WebWindow" );
ww.setName( "webWindow" );
ww.addSelfToJSContext();
ww.addParentObjectsToJS();
ww.createWebPluginFactory();
ww.setAttributes( { DeveloperExtrasEnabled: true,
                    JavascriptEnabled: true,
                    LocalContentCanAccessFileUrls: true } ); 
ww.setEnableContextMenu( true );
ww.load( "./loadtest.html" );
ww.show();

} catch(e) {

Loco.console.println(e);

}
