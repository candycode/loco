try {

var ww = Loco.gui.create( "WebWindow" );
ww.addObjectToContext( Loco.gui, "gui" );
ww.createWebPluginFactory();
ww.setAttributes( { DeveloperExtrasEnabled: true,
                    JavascriptEnabled: true,
                    LocalContentCanAccessFileUrls: true } ); 
ww.setEnableContextMenu( true );

if( Loco.ctx.os() === "WINDOWS" )
  ww.load( "./loadtest-win.html" );
else
  ww.load( "./loadtest.html" );

ww.show();

} catch(e) {

Loco.console.println(e);

}
