try {

//http://maps.googleapis.com/maps/api/staticmap?center=Brooklyn+Bridge,New+York,NY&zoom=14&size=512x512&maptype=roadmap
//&markers=color:blue%7Clabel:S%7C40.702147,-74.015794&markers=color:green%7Clabel:G%7C40.711614,-74.012318
//&markers=color:red%7Ccolor:red%7Clabel:C%7C40.718217,-73.998284&sensor=false
  var ctx = Loco.ctx;
  var urlparams = {query:{center: "Brooklyn Bridge,New York,NY".replace( / /g, '+' ),
                          zoom: 14,
                          size: "1024x1024",
                          maptype:"roadmap",
                          sensor: false,
                          markers: ['color:blue|label:L|40.702147,-74.015794',
                                    'color:red|label:O|40.711614,-74.012318']}};
// create web  window  
  var ww = Loco.gui.create( "WebWindow" );
  ctx.onError.connect( function( err ) { Loco.gui.criticalDialog( "Error", err ); ctx.exit( -1 ); } ); 
  ww.setPageSize( 512, 512 );
  ww.configScrolling( {h:"off",v:"off"} );   
  ww.addParentObjectsToJS();
  ww.evaluate = function( f ) {
    return this.eval( '(' + f.toString() + ')();' );
  }
  // sync load
  if( !ww.syncLoad( "http://maps.googleapis.com/maps/api/staticmap", urlparams, 10000 ) ) {
    throw( "Error" );
  }
  var e = ww.evaluate( function() { return document.getElementsByTagName('img'); } );
  var i;
  for( i in e[ 0 ] ) Loco.console.println( i + ": " + e[ 0 ][ i ] );
  ww.saveSnapshot( "gmaps-snapshot.png" );
  ww.saveToPDF( "gmap-snapshot.pdf" );
  ctx.exit( 0 );
} catch(e) {
  Loco.console.printerrln(e);
  Loco.ctx.exit( -1 );
}

