try {

  function printWkitConfig( attr ) {
    for( k in attr ) {
      Loco.console.println( k + ": " + attr[ k ] );
    }
  }
 
  var ww = Loco.gui.create( "WebWindow" );
  printWkitConfig( ww.getAttributes() );
  ww.setAddParentObjectsToJS( true );
  ww.load("http://www.geek.com");
  ww.show();
} catch(e) {
  Loco.console.printerrln(e);
}

