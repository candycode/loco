
var loco = new Loco;
var globalString = "GLOBAL STRING";
try {

loco.env.showWindow();
loco.read( "http://www.gazzetta.it", function(t) { document.write(t); } );

loco.env.exec();
} catch(e) {
  loco.cout.println(e);
}