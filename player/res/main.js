try {
Loco.console.println("Hello loco");
Loco.console.println("OS: " + Loco.ctx.os());
Loco.console.println("ENV: " );
var e = Loco.sys.env();
for( v in e ) {
    Loco.console.print( v );
    Loco.console.print( " = " );
    Loco.console.println( e[ v ] );
}
Loco.console.println("QT VERSION: " + Loco.ctx.qtVersion());
Loco.console.println( Loco.sys.run( "/cygwin/bin/ls" ) );
} catch(e) {
  Loco.console.printerrln( e );
}

