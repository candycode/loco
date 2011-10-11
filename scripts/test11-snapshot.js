try {

  var ctx = Loco.ctx;
// command line
  var cmdParam = ctx.cmdLine()[ctx.cmdLine().length - 1];
  var WEBSITE = cmdParam.lastIndexOf( ".js" ) < 0 ? 
                cmdParam : "http://www.nyt.com";  
  Loco.console.println( WEBSITE );
// create web  window  
  var ww = Loco.gui.create( "WebWindow" );
  ww.setPageSize( 4096, 4096 );
  ww.configScrolling( {h:"off",v:"off"} );   
  ww.setAttributes( {FrameFlatteningEnabled: true} ); //flatten frames 
// sync load
  if( !ww.syncLoad( WEBSITE, 10000 ) ) {
    throw( "Error loading website " + WEBSITE );
  }
  ww.saveSnapshot( "websnapshot-sync.png" );
  ww.saveToPDF( "websnapshot-sync.pdf" );
// async load
  ww.loadFinished.connect( function( ok ) { 
    if( ok ) {
        ww.saveSnapshot( "websnapshot.png" );
        ww.saveToPDF( "websnapshot.pdf" );
        Loco.ctx.quit();//exit from the event loop
    } else Loco.gui.errorDialog( "Error loading page" );
  });
  ctx.onError.connect( function( err ) { Loco.gui.criticalDialog( "Error", err ); ctx.exit( -1 ); } ); 
  ww.load(WEBSITE);
} catch(e) {
  Loco.console.printerrln(e);
  Loco.ctx.exit( -1 );
}

