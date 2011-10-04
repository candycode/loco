try {

var ww = Loco.gui.create( "WebWindow" );
Loco.ctx.onError.connect( function(e) {Loco.console.println(e);});
ww.addParentObjectsToJS();
ww.createWebPluginFactory();
ww.setAttributes( { DeveloperExtrasEnabled: true,
                    LocalContentCanAccessFileUrls: true } ); 
ww.setEnableContextMenu( true );
ww.load( "./loadtest.html" );
ww.show();

} catch(e) {

Loco.console.println(e);

}
