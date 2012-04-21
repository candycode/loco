try {

var gui = Loco.gui.create;

var parentWidget = gui( "Widget" );
var layout = gui( "HBoxLayout" );
var childWidget1 = gui( "WebWindow" );
var childWidget2 = gui( "WebWindow" );
layout.addWidget( childWidget1 ); // is it possible to 'return this' from native methods ???
layout.addWidget( childWidget2 ); 
parentWidget.setLayout( layout );
childWidget1.load( "http://www.google.com" );
childWidget2.load( "http://www.yahoo.com"  );
parentWidget.show();


} catch( e ) {
  Loco.console.printerrln( e );
  Loco.ctx.exit( -1 );
}
