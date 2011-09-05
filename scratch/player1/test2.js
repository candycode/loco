<<<<<<< HEAD
try {
var loco = new Loco;
//loco.cout.println(loco.p);
loco.p("CIAO");
loco.cbacks.push(function(){loco.cout.println("cback");});
Loco.prototype.cbacks.pop()();
loco.env.exec();
} catch(e) {
loco.cout.println(e);
}
=======

var loco = new Loco;
try {
loco.cout.println("starting");
//loco.cout.println( loco.env.read( "http://www.google.com" ) );
loco.env.showWindow();
//loco.env.setHtml( loco.env.read( "http://www.google.com" ) );
loco.env.load( "http://www.google.com", "" );
loco.env.exec();
} catch(e) {
loco.cout.println(e);
}
>>>>>>> origin
