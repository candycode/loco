try {
// prelude
  var $include = Loco.ctx.include;
  var print = Loco.console.println;
  var ctx = Loco.ctx;

// command line
  var cmdParam = ctx.cmdLine()[ctx.cmdLine().length - 1];
  var WEBSITE = cmdParam.lastIndexOf( ".js" ) < 0 ? 
                cmdParam : "http://www.cscs.ch";  

// create main window  
  var ww = Loco.gui.create( "WebWindow" );
 // setup main window 
  ww.setAttributes( {DeveloperExtrasEnabled: true,
                     LocalContentCanAccessFileUrls: true,
                     LocalContentCanAccessRemoteUrls: true,
                     AcceleratedCompositingEnabled: true } );
  ww.setEnableContextMenu( true );
  ww.setForwardKeyEvents( true );
  if( !ww.syncLoad( WEBSITE, 5000 ) ) throw "Load failure";
  var elements = ww.findElements( "div" );
  print( "Number of elements: " + elements.length );
  print( "Attribute names: " + elements[ 0 ].attributeNames() );
  print( "Classes: " + elements[ 0 ].classes() );
  print( "Element 0 id: " + elements[ 0 ].eval( "this.id" ) );
  print( "Element 0 opacity: " + elements[ 0 ].styleProperty( "opacity" ) );
  print( "Element 0 tag name: " + elements[ 0 ].tagName() ); 
  elements = ww.forEachElement( "*", "this.childNodes.length === 0" );
  print( elements.length );
  elements = ww.forEachElement( "div", "this.style.backgroundColor='yellow'; false;" );
  print( elements.length );
  elements = ww.forEachElement( "div", "this.style['-webkit-transform']='rotate(1deg)'; false;" ); 
  print( elements.length );
  ww.show();
} catch(e) {
  Loco.console.printerrln(e);
  Loco.ctx.exit( -1 );
}

