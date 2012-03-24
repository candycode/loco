try {
  var print = Loco.console.println;
  var ctx = Loco.ctx;
  Loco.ctx.include( 'xmlToJSON.js' ); 

// command line
  var cmdParam = ctx.cmdLine()[ctx.cmdLine().length - 1];
  var PAGE = cmdParam.lastIndexOf( ".js" ) < 0 ? 
                cmdParam : "./test15.xml";  
 
  var parser=new DOMParser();
  var xmlDoc=parser.parseFromString( Loco.ctx.read( PAGE ), "text/xml" );
  var json = xmlToJson( xmlDoc );
  print( jsonToString( json ) );
  Loco.ctx.exit( 0 );
} catch(e) {
  Loco.console.printerrln(e);
  Loco.ctx.exit( -1 );
}

