Loco.console.println("Hello loco");

try {
  var menu = function() {
    return {
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
  };
  var ww = Loco.gui.create( "WebMainWindow" );

  ww.setAttribute( "AutoLoadImages", false );
  ww.setAddParentObjectsToJS( true );
  ww.setContentEditable( true );
  ww.setMenu( menu() );
  ww.loadProgress.connect( function( i ) { ww.setStatusBarText( i + "%" ); } );
  ww.loadFinished.connect( function( ok ) { 
    if( ok ) {
      ww.setStatusBarText( "DONE" );
      //ww.eval( "alert('done')");
    } 
  } );
  ww.selectionChanged.connect( function() {
    Loco.console.println( ww.selectedText() );  
  } );
  ww.load("http://www.wsj.com");
  ww.show();
  ww.setStatusBarText( "Loading..." );
  Loco.ctx.eventLoop();
  /*ww.closing.connect( el.quit );
  ww.load("http://www.wsj.com");
  ww.show(); el.exec();
  ww.setHtml("");
  Loco.sys.sleep( 5000 );
  ww.load("http://lastampa.it");
  ww.show(); el.exec();*/
  //Loco.ctx.exec();
} catch(e) {
  Loco.console.printerrln(e);
}

