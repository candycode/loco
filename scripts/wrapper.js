try {
// prelude
  var $include = Loco.ctx.include;
  var print = Loco.console.println;
  var ctx = Loco.ctx;

  $include( 'keys.js' ); 


// main window menu
  var menu = {
     "Exit": {
      "Quit": {
          "cback"  : "Loco.webWindow.close()",
          "tooltip": "Quit",
          "status" : "Exit from application",
          "icon"   : ""
       }
     }
    };
 
// command line
  var cmdParam = ctx.cmdLine()[ctx.cmdLine().length - 1];
  var WEBSITE = cmdParam.lastIndexOf( ".js" ) < 0 ? 
                cmdParam : "http://www.nyt.com";  

// create main window  
  var ww = Loco.gui.create( "WebMainWindow" );
  ww.setName( "webWindow" ); 
  ww.addSelfToJSContext();
  ww.addParentObjectsToJS();  
 // setup main window 
  ww.setMenu( menu );
  ww.setAttributes( {JavascriptEnabled: true,
                     PluginsEnabled: true, 
                     DeveloperExtrasEnabled: true,
                     LocalContentCanAccessFileUrls: true,
                     LocalContentCanAccessRemoteUrls: true,
                     WebGLEnabled: true,
                     AcceleratedCompositingEnabled: true } );
  ww.setEnableContextMenu( true );
//  ww.loadProgress.connect( function( i ) { ww.setStatusBarText( i + "%" ); } );
  ww.setForwardKeyEvents( true );
//  ww.keyPress.connect( function( k, m, c ) { 
//                         if( k === LocoKey.F11 ) ww.showNormal();
//                       } );
//  ww.loadFinished.connect( function( ok ) { 
//    if( ok ) ww.eval("Loco.webWindow.setStatusBarText('DONE');");
//    else Loco.gui.errorDialog( "Error loading page" );
//  });
  ww.setStatusBarText( "Loading..." );
  ww.setWindowTitle( ctx.appName() ); 
  ctx.onError.connect( function( err ) { Loco.gui.criticalDialog( "Error", err ); ctx.exit( -1 ); } ); 
  ww.show(); 
  ww.load(WEBSITE);
  
} catch(e) {
  Loco.console.printerrln(e);
  Loco.ctx.quit( -1 );
}

