try {
  var ctx = Loco.ctx;
// command line
  var cmdParam = ctx.cmdLine()[ctx.cmdLine().length - 1];
  var WEBSITE = cmdParam.lastIndexOf( ".js" ) < 0 ? 
                cmdParam : "http://www.nyt.com";  

// create main window  
  var ww = Loco.gui.create( "WebWindow" );
  ww.setPageSize( 600, 800 );   
  ww.loadFinished.connect( function( ok ) { 
    if( ok ) ww.saveToPDF( "websnapshot.pdf" );
    else Loco.gui.errorDialog( "Error loading page" );
  });
  ctx.onError.connect( function( err ) { Loco.gui.criticalDialog( "Error", err ); ctx.exit( -1 ); } ); 
  ww.load(WEBSITE);
  
} catch(e) {
  Loco.console.printerrln(e);
  Loco.ctx.exit( -1 );
}

