try {
  var print = Loco.console.println;
  var ctx = Loco.ctx;
  Loco.ctx.include( 'xmlToJSON.js' ); 

  function traverse( dom, indent ) {
    indent  = indent || "";  
    for( i in dom ) {
      if( typeof dom[ i ] !== 'object' && dom[ i ] ) print( indent + i + ": " + dom[ i ] );
      else if( dom[ i ] ) {
        print( indent + i + ":\n" );
        traverse( dom[ i ], indent + "  " );
      }
    }
  }

// command line
  var cmdParam = ctx.cmdLine()[ctx.cmdLine().length - 1];
  var WEBSITE = cmdParam.lastIndexOf( ".js" ) < 0 ? 
                cmdParam : "http://www.nyt.com";  
 
  var parser=new DOMParser();
  var xmlDoc=parser.parseFromString( Loco.ctx.read( WEBSITE ), "text/xml" );
  print( xmlDoc.getElementsByTagName( "author" ).length );
  print( xmlDoc.getElementsByTagName( "author" )[ 0 ].textContent );
  //print( xmlDoc.documentElement.childNodes );
  var json = xmlToJson( xmlDoc );
  print( jsonToString( json ) );
  Loco.ctx.exit( 0 );

  
// create main window  
  var ww = Loco.gui.create( "WebWindow" );
 // setup main window 
  ww.setAttributes( {LocalContentCanAccessFileUrls: true,
                     LocalContentCanAccessRemoteUrls: true} );
  if( !ww.syncLoad( WEBSITE, 5000 ) ) throw "Load failure";

  var elements = ww.findElements( "tag1" );
  print( elements.length );
  print( elements[ 0 ].attributeNames() );
  print( elements[ 0 ].eval( "this.id1" ));
  Loco.ctx.exit( 0 ); 
} catch(e) {
  Loco.console.printerrln(e);
  Loco.ctx.exit( -1 );
}

