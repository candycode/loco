try {
// prelude
  var $include = Loco.ctx.include;
  var print = Loco.console.println;
  var ctx = Loco.ctx;

// command line
  var cmdParam = ctx.cmdLine()[ctx.cmdLine().length - 1];
  var WEBSITE = cmdParam.lastIndexOf( ".js" ) < 0 ? 
                cmdParam : "http://www.marinij.com";  

// create main window  
  var ww = Loco.gui.create( "WebWindow" );
 // setup main window 
  ww.setAttributes( {DeveloperExtrasEnabled: true,
                     LocalContentCanAccessFileUrls: true,
                     LocalContentCanAccessRemoteUrls: true,
                     AcceleratedCompositingEnabled: true } );
  ww.setEnableContextMenu( true );
  ww.setForwardKeyEvents( true );
  if( !ww.syncLoad( WEBSITE, 10000 ) ) throw "Load failure";
  var elements = ww.findElements( "div" );
  print( "Number of elements: " + elements.length );
  print( "Element 0 attributes: " + elements[ 0 ].attributeNames() );
  print( "Element 0 classes: " + elements[ 0 ].classes() );
  print( "Element 0 id: " + elements[ 0 ].eval( "this.id" ) );
  print( "Element 0 opacity: " + elements[ 0 ].styleProperty( "opacity" ) );
  print( "Element 0 tag name: " + elements[ 0 ].tagName() ); 
  elements = ww.forEachElement( "*", "this.childNodes.length === 0" );
  print( "Number of leaves: " + elements.length );
  // forEachElement uses the value returned from the per-element evaluated code
  // to decide if the element should be added to the list of returned elements;
  // applying a function that returns false or specifying 'false' as the last
  // statement of the evaluated code results in no element added to the returned element list
  elements = ww.forEachElement( "div", "this.style.backgroundColor='yellow'; false;" );
  elements = ww.forEachElement( "div", "this.style['-webkit-transform']='rotate(1deg)'; false;" ); 
  ww.show();
} catch(e) {
  Loco.console.printerrln(e);
  Loco.ctx.exit( -1 );
}

