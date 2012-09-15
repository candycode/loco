try {
var mw = Loco.gui.create("MainWindow");
var gl = Loco.ctx.loadQtPlugin(Loco.ctx.args[2]);
//uncomment to load overlay
//gl.loadPage( "./hud.html", true );
mw.setCentralWidget(gl);
mw.show();
} catch(e){
  Loco.console.printerrln( e.message || e );
  Loco.ctx.exit(-1);
}
