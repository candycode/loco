try {
  var print = Loco.console.println;
  var ctx = Loco.ctx;
  Loco.ctx.include( 'xmlToJSON.js' ); 

/*  function traverse( dom, indent ) {
    indent  = indent || "";  
    for( i in dom ) {
      if( typeof dom[ i ] !== 'object' && dom[ i ] ) print( indent + i + ": " + dom[ i ] );
      else if( dom[ i ] ) {
        print( indent + i + ":\n" );
        traverse( dom[ i ], indent + "  " );
      }
    }
  }
*/

// command line
  var cmdParam = ctx.cmdLine()[ctx.cmdLine().length - 1];
  var WEBSITE = cmdParam.lastIndexOf( ".js" ) < 0 ? 
                cmdParam : "http://www.nyt.com";  
 
  var parser=new DOMParser();
  var xmlDoc=parser.parseFromString( Loco.ctx.read( WEBSITE ), "text/xml" );
  var json = xmlToJson( xmlDoc );
  print( jsonToString( json ) );
  Loco.ctx.exit( 0 );
} catch(e) {
  Loco.console.printerrln(e);
  Loco.ctx.exit( -1 );
}

