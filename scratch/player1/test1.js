loco = new Loco;
alert("ciao");
loco.cout.println( loco.web.readTxt( "test1.js" ) );
loco.web.setHtml( "<head><body><script>alert('got it');</script></body></html>", "default");
/*loco = new Loco;
loco.web.loadHtml("http://www.google.com", "default", "loco.cout.println('loaded');");
loco.cout.println('ola');
loco.web.setHtml( "<html><head><script>(new Loco).cout.println('eheh');</script></head><body>Ciauuu</body></html>", "default" );
loco.cout.println("lalalala");*/
