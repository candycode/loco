try {
// prelude
  var $include = Loco.ctx.include;
  var print = Loco.console.println;
  var ctx = Loco.ctx;

// command line
  var cmdParam = ctx.cmdLine()[ctx.cmdLine().length - 1];
  var URL = cmdParam.lastIndexOf( ".js" ) < 0 ? 
                cmdParam : "http://www.google.com";  

// create main window  
  var ww = Loco.gui.create( "WebWindow" );

  var ok = ww.saveUrl( "ftp://ftp.cscs.ch/out/molekel/molekel_5.4/molekel_5_4_0_linux_x86_64.tar.gz",
              "dload", 20000 );   
  
  if( !ok ) throw( "Error downloading" );
  Loco.ctx.exit( 0 );

} catch(e) {
  Loco.console.printerrln(e);
  Loco.ctx.exit( -1 );
}

