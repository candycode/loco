Loco.console.println("Hello loco");
try {
  var el = Loco.sys.eventLoop();
  var ww = Loco.gui.create( "WebWindow" );
  ww.closing.connect( el.quit );
  ww.load("http://www.wsj.com");
  ww.show(); el.exec();
  ww.setHtml("");
  Loco.sys.sleep( 5000 );
  ww.load("http://lastampa.it");
  ww.show(); el.exec();
  //Loco.ctx.exec();
} catch(e) {
  Loco.console.printerrln(e);
}

