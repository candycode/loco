Loco.console.println("Hello loco");
try {
  var ww = Loco.gui.create( "WebWindow" );
  ww.load("http://www.wsj.com");
  ww.show(); 
  Loco.ctx.exec();
} catch(e) {
  Loco.console.printerrln(e);
}

