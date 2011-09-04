
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
