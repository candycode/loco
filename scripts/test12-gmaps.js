try {
  var ctx = Loco.ctx;
  var urlparams = {query:{center: "Brooklyn Bridge,New York,NY".replace( / /g, '+' ),
                          zoom: 14,
                          size: "1024x1024",
                          maptype:"roadmap",
                          sensor: false,
                          markers: ['color:blue|label:L|40.702147,-74.015794',
                                    'color:red|label:O|40.711614,-74.012318']}};
  ctx.onError.connect( function( err ) { Loco.gui.criticalDialog( "Error", err ); ctx.exit( -1 ); } ); 
// create web  window  
  var ww = Loco.gui.create( "WebWindow" );
  ww.setPageSize( 512, 512 );
  ww.configScrolling( {h:"off",v:"off"} );   
  ww.evaluate = function( f ) {
    return this.eval( '(' + f.toString() + ')();' );
  }
// sync load
  if( !ww.syncLoad( "http://maps.googleapis.com/maps/api/staticmap", urlparams, 10000 ) ) {
    throw( "Error" );
  }
  var e = ww.eval( "document.getElementsByTagName('img')" );
  if( e.length < 1 ) throw "No 'img' tag found";
  var i;
  for( i in e[ 0 ] ) Loco.console.println( i + ": " + e[ 0 ][ i ] );
  ww.saveSnapshot( "gmaps-snapshot.png" );
  ww.saveToPDF( "gmap-snapshot.pdf" );
  ctx.exit( 0 );
} catch(e) {
  Loco.console.printerrln(e);
  Loco.ctx.exit( -1 );
}

