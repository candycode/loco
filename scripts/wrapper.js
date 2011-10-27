// try e.g.  https://mail.google.com/mail/feed/atom -u <username> -p <password>

try {
// prelude
  var $include = Loco.ctx.include;
  var print = Loco.console.println;
  var ctx = Loco.ctx;
  var logRequests = false;

  $include( 'keys.js' ); 

  //ctx.onError.connect( function( err ) { Loco.gui.criticalDialog( "Error", err ); ctx.exit( -1 ); } ); 


// main window menu
  var menu = {
     "Exit": {
      "Quit": {
          "cback"  : "Loco.webWindow.close();",
          "tooltip": "Quit",
          "status" : "Exit from application",
          "icon"   : ""
       }
     }
    };
  var userName, pwd, URL;
  var atom = false;
  for( var i = 0; i != ctx.cmdLine().length; ++i ) {
    if( ctx.cmdLine()[ i ] === "-u" && i < ctx.cmdLine().length - 1 ) {
      userName = ctx.cmdLine()[ i + 1 ];
    }
    if( ctx.cmdLine()[ i ] === "-p" && i < ctx.cmdLine().length - 1 ) {
      pwd = ctx.cmdLine()[ i + 1 ];
    }
    if( ctx.cmdLine()[ i ].match( /(^.+:\/\/.+)|(\S+\.htm.*$)/ ) ) {
      URL = ctx.cmdLine()[ i ];
    }
    if( ctx.cmdLine()[ i ] === "-atom" ) atom = true; 
  }

  if( !URL ) URL = "http://www.nyt.com";  

// create main window  
  var ww = Loco.gui.create( "WebMainWindow" );
  ww.onError.connect( function( err ) { print( err ); } ); 
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
  
  ctx.setDefaultSSLExceptionHandler();
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
  ww.loadFinished.connect( function( ok ) { 
    if( ok ) {
      var c = "Loco.webWindow.setStatusBarText('DONE: ' + \
               Loco.webWindow.totalBytes() + ' bytes loaded');";
      ww.eval( c );
      if( atom ) {
        var doc = "<html><head>"
        doc += "<style type=\"text/css\">\n"
        doc += "ul li {\nlist-style-image: none;\n list-style-type: none;\n }\n</style></head><body>";
        var elements = ww.findElements( 'entry' );
        for( var e = 0; e != elements.length; ++e ) {
          var title = elements[ e ].findFirstElement( 'title' ).eval( "this.textContent" );
          var summary = elements[ e ].findFirstElement( 'summary' ).eval( "this.textContent" );
          doc += '<br/><ul><li><h3>Title</h3> ' + title   + '</li>';
          doc += '<li><h4>Summary</h4> '   + summary + '</li></ul>'; 
        }
        doc += "</body></html>";
        atom = false;
        ww.setHtml( doc );
       
        //var parser = new DOMParser();
        //var xmlDoc = parser.parseFromString( ww.toHtml(), "text/xml" );
        //Loco.ctx.include( 'xmlToJSON.js' );
        //var json = xmlToJson( xmlDoc );
        //print( jsonToString( json ) );
        
        /*var html = xmlTraverse( xmlDoc, function( n ) {
                                           var doc = this.doc | "";
                                           if( n.nodeName === "entry" ) {
                                             doc += "<p>\n"
                                             doc += "<ul>\n";
                                             var title  n.getElementsByTagName( "title" )  */   
      }
    }
    else Loco.gui.errorDialog( "Error loading page" );
  });
  ww.setStatusBarText( "Loading..." );
  ww.setWindowTitle( ctx.appName() ); 
  ww.load( URL, {username: userName, password: pwd} ); 
  ww.show(); 
  
  
} catch(e) {
  Loco.console.printerrln(e);
  Loco.ctx.quit( -1 );
}

