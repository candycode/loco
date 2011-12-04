try {

  var $include = Loco.ctx.include;
  $include( 'keys.js' );
  var print = Loco.console.println;
  var ctx = Loco.ctx;
  function printWkitConfig( attr ) {
    for( k in attr ) {
      print( k + ": " + attr[ k ] );
    }
  }

  var menu = {
      "File": {
        "Open": {
          "cback"  : "alert('Open')",
          "tooltip": "Open file",
          "status" : "Opens a file"
        },
        "Open recent": {
          "cback"  : "alert('Open recent')",
          "tooltip": "Open recent",
          "status" : "Opens a recently opened file"
        }
      },
      "Test": {
        "Parent context": {
          "cback"  : "terminal.println('hello')",
          "tooltip": "Call parent context",
          "status" : "Call a parent context's function"
        }
      }   
    };
 
  
  var ww = Loco.gui.create( "Webindow" );
  ww.addObjectToContext( ww, "webWindow" );
  ww.eval( "alert(" + printWkitConfig( ww.getAttributes() + ")" );
  var mw = Loco.gui.create( "MainWindow" );
  mw.setCentralWidget( ww );
  mw.setMenu( menu );
  ww.setAttributes( {DeveloperExtrasEnabled: true } );
  ww.setEnableContextMenu( true );
  ww.loadProgress.connect( function( i ) { mw.setStatusBarText( i + "%" ); } );
  ww.keyPress.connect( function( k, m, c ) { 
      Loco.console.println( "Key pressed: " + k  );
     } ); 
  ww.loadStarted.connect( function() { ww.eval("var MyGlobal='MyGlobal'"); } );
  ww.loadFinished.connect( function( ok ) { 
    if( ok ) {
      var html = '\
      <html><head></head><body> \
      <h1>DONE!!!</h1>   \
      </body></html>';
      ww.setStatusBarText( "DONE" );
      ww.eval( 'webWindow.showFullScreen()' );  
      ww.eval( "gui.infoDialog( 'Frame name', webWindow.frameName() )");
      ww.eval( "terminal.println(MyGlobal)" );
    } 
  } );
  ww.selectionChanged.connect( function() {
    Loco.console.println( ww.selectedText() );  
  } );
  ww.setStatusBarText( "Loading..." );
  mw.setWindowTitle( ctx.appName() ); 
  ctx.onError.connect( function( err ) { Loco.gui.criticalDialog( "Error", err ); ctx.exit( -1 ); } );
  ww.show(); 
  ww.load("http://www.nyt.com");
  
} catch(e) {
  Loco.console.printerrln(e);
  Loco.ctx.exit( -1 );
}

