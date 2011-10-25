try {
// prelude
  var $include = Loco.ctx.include;
  var print = Loco.console.println;
  var ctx = Loco.ctx;
  var logRequests = true;

  $include( 'keys.js' ); 

  ctx.onError.connect( function( err ) { Loco.gui.criticalDialog( "Error", err ); ctx.exit( -1 ); } ); 

// main window menu
  var menu = {
     "Exit": {
      "Quit": {
          "cback"  : "Loco.webWindow.close(); Loco.ctx.exit( 0 );",
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
  ww.onError.connect( function( err ) { throw err; } ); 
  ww.setName( "webWindow" ); 
  ww.addSelfToJSContext();
  ww.addParentObjectsToJS();
  if( logRequests ) {
    ww.emitRequestSignal( true );
    ww.onRequest.connect( function(r) {
                            var i;
                            print( "===================================\n" );
                            for( i in r ) {
                              if( r[ i ] && typeof r[ i ] !== 'object' ) print( i + ': ' + r[ i ] );
                            } 
                          } );
  }
  ww.statusBarMessage.connect( function( msg ) { ww.setStatusBarText( msg ); } );
  ww.downloadRequested.connect( function( f ) {
                                  var filename = f.slice( f.lastIndexOf( '/' ) + 1 );
                                  filename = Loco.fs.curDir() + '/' + filename;
                                  var extension = filename.slice( filename.lastIndexOf( '.' ) );
                                  var fname = Loco.gui.saveFileDialog( "Save file", 
                                                filename, "(*" + extension + ")" );
                                  if( !fname ) return;
                                  if( !ww.saveUrl( f, fname, 20000 ) ) throw "Cannot save file";
                                  ww.reload();
                                } );
//ww.unsupportedContent.connect( function( c ) { Loco.console.println( c ); } );
  ww.unsupportedContent.connect( ww.downloadRequested );
  ww.fileDownloadProgress.connect( function( b, t ) {
                                     ww.setStatusBarText( b  + ( t > 0 ? ' of ' + t : "" ) );
                                   } ); 
  ww.closing.connect( function() { Loco.ctx.quit(); } );       
// setup main window 
  ww.setMenu( menu );
  ww.javaScriptConsoleMessage.connect( function( msg, lineno, srcid ) {
                                         print( lineno + ': ' + msg + '\n' + srcid ); } ); 
  ww.setAttributes( {JavascriptEnabled: true,
                     JavaEnabled: true,
                     JavascriptCanOpenWindows: true, 
                     PluginsEnabled: true, 
                     AutoLoadImages: true, 
                     DeveloperExtrasEnabled: true,
                     SpatialNavigationEnabled: true,
                     LocalContentCanAccessFileUrls: true,
                     LocalContentCanAccessRemoteUrls: true,
                     OfflineWebApplicationCacheEnabled: true,
                     SiteSpecificQuirksEnabled: true,
                     OfflineStorageDatabaseEnabled: true,
                     LocalStorageEnabled: true,
                     SiteSpecificQuirksEnabled: true,
                     WebGLEnabled: true,
                     XSSAuditingEnabled: false,
                     FrameFlatteningEnabled: true, 
                     AcceleratedCompositingEnabled: true} );

  ww.setEnableContextMenu( true );
  //ww.loadProgress.connect( function( i ) { ww.setStatusBarText( i + "%" ); } );
  ww.setForwardKeyEvents( true );
  ww.keyPress.connect( function( k, m, c ) { 
                           if( k === LocoKey.F11 ) ww.toggleFullScreen();
                       } );
  /*ww.loadFinished.connect( function( ok ) { 
    var c = "Loco.webWindow.setStatusBarText('DONE: ' + \
             Loco.webWindow.totalBytes() + ' bytes loaded');";
    if( ok ) ww.eval( c );
    else Loco.gui.errorDialog( "Error loading page" );
  });*/
  //ww.setStatusBarText( "Loading..." );
  ww.setWindowTitle( ctx.appName() ); 
  ww.load(WEBSITE, 5000); 
  ww.show(); 
  
  
} catch(e) {
  Loco.console.printerrln(e);
  Loco.ctx.quit( -1 );
}

