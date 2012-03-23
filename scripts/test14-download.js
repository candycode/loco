try {
  var ww = Loco.gui.create( "WebWindow" );
  ww.fileDownloadProgress.connect( function( a, b ) {
    Loco.console.printr( a );
  }) 
  var PREFIX = "test14-download";
  if( !Loco.fs.exists( PREFIX ) ) Loco.fs.mkdir( PREFIX );
  var ok = ww.saveUrl( "ftp://ftp.cscs.ch/out/molekel/molekel_5.4/molekel_5_4_0_linux_x86_64.tar.gz",
              PREFIX + "/dload", 20000 );    
  Loco.console.println(); 
  if( !ok ) throw( "Error downloading" );
  Loco.ctx.exit( 0 );

} catch(e) {
  Loco.console.printerrln(e);
  Loco.ctx.exit( -1 );
}

