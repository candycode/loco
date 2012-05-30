try {
var ppath = "";
if( Loco.ctx.os() === "WINDOWS" ) 
  ppath = "/cmakebuilds/project-loco/modules/plugins/glview/Release/glview.dll";
else
  ppath = Loco.ctx.homeDir() + "/build/loco/modules/plugins/glview/libglview.so";
var mw = Loco.gui.create("MainWindow");
mw.setCentralWidget( Loco.ctx.loadQtPlugin(ppath));
mw.show();
} catch(e){
  Loco.console.printerrln( e.message || e );
  Loco.ctx.exit(-1);
}
