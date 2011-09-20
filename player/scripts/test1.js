Loco.console.println("Hello loco");
Loco.console.println("OS: " + Loco.ctx.os());
Loco.console.println("ENV: " );
var e = Loco.sys.env();
for( v in e ) {
    Loco.console.print( v );
    Loco.console.print( " = " );
    Loco.console.println( e[ v ] );
}
Loco.console.println("QT VERSION: " + Loco.sys.qtVersion());
//Loco.console.println( Loco.ctx.system( "/cygwin/bin/ls" ) );

