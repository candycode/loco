/*loco = new Loco;
loco.cout.println( "ciao" );
var a = "lala";
function ret(v){return v;}
ret(2);
loco = new Loco;
alert("ciao");
loco.cout.println( loco.web.readTxt( "test1.js" ) );
loco.web.setHtml( "<head><body><script>alert('got it');</script></body></html>", "default");*/
/*loco = new Loco;
loco.web.loadHtml("http://www.google.com", "default", "loco.cout.println('loaded');");
loco.cout.println('ola');
loco.web.setHtml( "<html><head><script>(new Loco).cout.println('eheh');</script></head><body>Ciauuu</body></html>", "default" );
loco.cout.println("lalalala");*/


//loco.load = function( url, cbk ) {
//    loco.loadcbacks.append( cbk );
//    loco.env.load( url, cbk === null ? false : true );
//}


loco = new Loco;
try {
loco.env.println("ciao");


loco.cout.println("loading url...");
loco.env.showWindow();
loco.env.load( "http://www.amazon.com", 
  "loco.env.load(\"http://www.yahoo.com\",\"\");" );
//loco.cout.println("1");
//loco.env.sleep(8000);
//loco.cout.println("2");
//loco.env.load( "http://www.yahoo.com", '' );
loco.env.exec();
} catch(e) {
loco.cout.println(e);

}