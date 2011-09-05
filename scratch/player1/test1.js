
var loco = new Loco;
var globalString = "GLOBAL STRING";
try {
//this throws an error
//loco.env.println("ciao");
loco.cout.println("loading url...");
loco.env.showWindow();
//loco.env.load( "http://www.google.com", 
//  "loco.env.sleep(8000);loco.env.load(\"http://www.yahoo.com\",\"\");" );
//loco.cout.println(loco.toSource());
var f = function(){
               lococout__.println("...done!");
               lococout__.println(globalString);  
             }  
loco.load( "http://www.google.com",
           function() { 
             loco.cout.println( "...loaded, waiting..." );
             loco.env.sleep(4000);
             loco.cout.println(globalString);   
             loco.cout.println("...waited...loading...");
             
             loco.env.load( "http://www.yahoo.com"," \
               document.write('<p><p>************************************************************************</p></p>'); \
               lococout__.println('...done!'); \
               lococout__.println(document.toString());"  
                );
            });           
loco.env.exec();
} catch(e) {
loco.cout.println(e);

}

/*
var v = loco.create("WebView",{localaccess: true} );
v.show(...);
v.executeScript("http://arielrepo.net/.../myscript",["default","lint"]);
//v.loadPage("...");
loco.exec();
