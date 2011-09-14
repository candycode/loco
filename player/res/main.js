try {
  Loco.console.println("Hello loco!");
  var app = Loco.ctx.appInfo();
  for( p in app ) {
    if( p !== null && app[ p ].length > 0 ) { 
      Loco.console.println( p + ": " + app[ p ] );
    }     	
  }

  Loco.console.println("OS: " + Loco.ctx.os());
  Loco.console.println("ENV: " );
  var e = Loco.sys.env();
  for( v in e ) {
    Loco.console.print( v );
    Loco.console.print( " = " );
    Loco.console.println( e[ v ] );
  }
  Loco.console.println("QT VERSION: " + Loco.ctx.qtVersion());
  if( Loco.ctx.os() === "WINDOWS" )
    Loco.console.println( Loco.sys.run( "cmd.exe", ["/C","dir"] ) );    
  else
    Loco.console.println( Loco.sys.run( "ls" ) );
} catch(e) {
  Loco.console.printerrln( e );
}

