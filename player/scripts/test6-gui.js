try {

  var print = Loco.console.println;

  function printWkitConfig( attr ) {
    for( k in attr ) {
      Loco.console.println( k + ": " + attr[ k ] );
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
          "cback"  : "Loco.console.println('hello')",
          "tooltip": "Call parent context",
          "status" : "Call a parent context's function"
        }
      }   
    };
 
  
  var ww = Loco.gui.create( "WebMainWindow" );
  printWkitConfig( ww.getAttributes() );
  ww.setAddParentObjectsToJS( true );
  ww.setMenu( menu );
  ww.setAttributes( {DeveloperExtrasEnabled: true } );
  ww.setEnableContextMenu( true );
  ww.loadProgress.connect( function( i ) { ww.setStatusBarText( i + "%" ); } );
  ww.loadFinished.connect( function( ok ) { 
    if( ok ) {
      var html = '\
      <html><head></head><body> \
      <h1>DONE!!!</h1>   \
      </body></html>';
      ww.setStatusBarText( "DONE" );
      ww.eval( "document.onkeypress = function( k ) { \
                if( k.keyCode == 27 ) LocoWebMainWindow.showNormal(); }" );
      ww.eval( 'LocoWebMainWindow.showFullScreen()' );  
      ww.eval( "alert('done')");
      //ww.eval( "document.write('" + html + "');" ); 
    } 
  } );
  ww.selectionChanged.connect( function() {
    Loco.console.println( ww.selectedText() );  
  } );
  ww.setStatusBarText( "Loading..." );
  ww.setWindowTitle( Loco.ctx.appName() ); 
  Loco.ctx.onError.connect( function( err ) { Loco.gui.criticalDialog( "Error", err ); Loco.ctx.exit(-1); } );
  ww.load("http://www.nyt.com");
  ww.show();
} catch(e) {
  Loco.console.printerrln(e);
}

