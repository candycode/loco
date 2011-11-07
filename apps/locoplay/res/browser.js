// try e.g. (ATOM) https://mail.google.com/mail/feed/atom -u <username> -p <password> -feed 
//          (RSS)  http://feeds.bbci.co.uk/news/world/rss.xml -feed
try {
// prelude
  var $include = Loco.ctx.include;
  var print = Loco.console.println;
  var ctx = Loco.ctx;
  var logRequests = false;
  var logActions = false;

  ctx.onError.connect( function( err ) { Loco.gui.warningDialog( "Error", err ); /*ctx.exit( -1 );*/ } );
  ctx.javaScriptConsoleMessage.connect( function( t, l, s) { print( t + ": " + l + "\n" + s ); } );
      // main window menu
  var menu = {
     "_1_Actions": {
       "_1_Toggle fullscreen": {
        cback: "Loco.webWindow.toggleFullScreen()",
        tooltip: "Toggle fullscreen",
        status: "Show fullscreen/normal",
        icon: ""
       },
       "_2_Quit": {
        cback  : "Loco.webWindow.close();",
        tooltip: "Quit",
        status : "Exit from application",
        icon   : ""
       }      
     }   
    };
  var userName, pwd, URL;
  var feed = false;
  var handleFeed = false;
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
    if( ctx.cmdLine()[ i ] === "-feed" ) { 
      feed = true;
      handleFeed = true;
    }
    if( ctx.cmdLine()[ i ] === "-logRequests" ) logRequests = true;
    if( ctx.cmdLine()[ i ] === "-logActions"  ) logActions  = true;
  }

  if( !URL ) URL = "http://www.google.com";  

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
                                         print( srcid + ', line ' + lineno + '> ' + msg ); } );                                
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
  ww.loadProgress.connect( function( i ) { ww.setStatusBarText( i + "%" ); } );
  ww.setForwardKeyEvents( true );
  
  if( feed ) {                     
    ww.setLinkDelegationPolicy( "all" );
    ww.linkClicked.connect( function( url ) { handleFeed = true; ww.load( url ); } );
  }
  ww.linkHovered.connect( function(url) { ww.setStatusBarText( url.toString() ); } ); 
  if( logActions ) {
    ww.webActionTriggered.connect( function( a, b ) { print( "ACTION " + a + " TRIGGERED" ); } );
    ww.setEmitWebActionSignal( true );
  }
  ww.loadFinished.connect( function( ok ) { 
    if( ok ) {
      var c = "Loco.webWindow.setStatusBarText('DONE: ' + \
               Loco.webWindow.totalBytes() + ' bytes loaded');";
      
      ww.eval( c );
      if( handleFeed ) {
        var doc = "<html><head>";
        doc += "<style type=\"text/css\">\n"
        doc += "ul li {\nlist-style-image: none;\nlist-style-type: none;\n }\n</style></head><body>";
        var entryTag, titleTag, textTag, linkTag, hrefEval;
        if( ww.eval( "document.getElementsByTagName('rss')" ).length > 0 ) { //RSS
          entryTag  = 'item';
          titleTag  = 'title';
          textTag   = 'description';
          linkTag   = 'link';
          hrefEval  = 'this.textContent';
        } else if( ww.eval( "document.getElementsByTagName('feed')" ).length > 0  ) { //ATOM
          entryTag  = 'entry';
          titleTag  = 'title';
          textTag   = 'summary';
          linkTag   = 'link';
          hrefEval  = 'this.getAttribute( "href" )';
        } else return;
        
        var elements = ww.findElements( entryTag );
        for( var e = 0; e != elements.length; ++e ) {
          var title = elements[ e ].findFirstElement( titleTag ).eval( "this.textContent" );
          var summary = elements[ e ].findFirstElement( textTag ).eval( "this.textContent" );
          var link = elements[ e ].findFirstElement( linkTag ).eval( hrefEval );
          doc += '<br/><ul><li><h3>Title</h3> ' + title   + '</li>';
          doc += '<li><a href="' + link + '">'  + 'LINK</a></li>';
          doc += '<li><h4>Summary</h4> '        + summary + '</li></ul>'; 
        }
        doc += "</body></html>";
        handleFeed = false;
        ww.setHtml( doc );
        ww.show();
      }
    }
    else Loco.gui.errorDialog( "Error loading page" );
  });
  ww.setStatusBarText( "Loading..." );
  ww.setWindowTitle( ctx.appName() ); 
  ww.load( URL, {username: userName, password: pwd} ); 
  if( !feed ) ww.show();   
} catch(e) {
  Loco.console.printerrln(e);
  Loco.ctx.quit( -1 );
}

