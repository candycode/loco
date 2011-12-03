try {

var buffer = "",
    line = "",
    checker = Loco.syntaxChecker,
    print = Loco.console.println,
    history = [],
    evalResult;
print( "\nLoCO JavaScript REPL - enter ':quit' to exit\n" );
Loco.ctx.onError.connect( function( msg ) { print( msg ); } );

function readLine() {
  var line = "";
  var c = "";
  while( true ) {
    c =  Loco.console.getChar();
    if( c === "\n" ) break;
    line += c;
  }
  return line;
}

while( true ) {
  while( !buffer || checker.incomplete( buffer ) ) {
    if( !buffer ) Loco.console.print( "> " );
    else Loco.console.print( ">>> " );
    line = readLine();
    if( line === ":quit" ) Loco.ctx.exit( 0 );
    else if( line ) buffer += "\n" + line;
  }
  if( checker.state() === "error" ) {
    Loco.console.println( "ERROR> " + checker.errorMessage() 
      + " at line " + ( checker.errorLineNumber() - 1 ) +
      ", column " + checker.errorColumnNumber() ); 
  } else {
    try {
      evalResult = eval( buffer );
      print( typeof evalResult === 'object' ? evalResult.toString() : evalResult  );
      history.push( buffer );
    } catch( e ) { print( e ); }
  }
  buffer = "";   
}

} catch( e ) {
  Loco.console.printerrln( e );
  Loco.ctx.exit( -1 );
}

