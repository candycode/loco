try {
 
Loco.ctx.onError.connect( function(e) { throw e; } );
var BiwaScheme = BiwaScheme || {};
var Console = {};
Console.puts = function(str, no_newline) {
  Loco.console.print(str);
  if (!no_newline) {
     Loco.console.print("\n");
  }
};
Console.p = function() {
  Loco.console.print.apply(this, arguments);
};

function puts(m,nnl) { return Console.puts( n, nnl ); }
Loco.ctx.include( "../filters/biwascheme/biwascheme-min.js" );
//BiwaScheme.Interpreter.dumper = new BiwaScheme.Dumper();

function show_error(e){
 Loco.console.println("Error: "+e.message);
}
function ev(str){
  Loco.console.println("ev> "+str);
  var ret = (new BiwaScheme.Interpreter(show_error)).evaluate(str);
  return ret;
}

BiwaScheme.define_libfunc("print", 1, 1, function(args) {
  
  Loco.console.println( args[ 0 ] );
  return true;
});
BiwaScheme.define_libfunc("open-web-page", 1, 1, function(args) {
  var ww = Loco.gui.create( "WebWindow" );
  puts( args[ 0 ] );
  ww.load( args[ 0 ] );
  ww.show();
  return true;
});

ev('(print "hi,...from a scheme function defined from javascript")');
var E = ev( "(+ 1 2 )" );
Loco.console.println( '\t' + E );
ev( '(js-eval "Loco.console.println( \'hi...from a javascript function invoked from scheme\' )")' );

ev( "(define (fib n)" +
    "  (if (< n 2)" +
    "    n" +
    "    (+ (fib (- n 1)) (fib (- n 2)))))" +
    "(print (fib 12))" );



ev( '(open-web-page "http://www.nyt.com")' );

//Loco.ctx.exit( 0 );
} catch(e) { 
  Loco.console.printerrln(e);
}


/*
//WITH THE FOLLOWING EXPLICIT INCLUDES IT WORKS WITH QtScript on Qt 4.8 as well, provided
//all 'var' keywords are removed from the global declarations
var dir = "../filters/biwascheme/";
include( dir+"version.js");
var include = Loco.ctx.include;
if( WKIT ) include( dir+"deps/jquery.js");
include( dir+"deps/underscore.js");
include( dir+"deps/underscore.string.js");
if( WKIT ) include( dir+"platforms/browser/console.js");
include( dir+"system/class.js");
include( dir+"stackbase.js");
include( dir+"system/set.js");
include( dir+"system/write.js");
include( dir+"system/pair.js");
include( dir+"system/value.js");
include( dir+"system/symbol.js");
include( dir+"system/char.js");
include( dir+"system/number.js");
include( dir+"system/port.js");
include( dir+"system/record.js");
include( dir+"system/hashtable.js");
include( dir+"system/syntax.js");
include( dir+"system/types.js");
include( dir+"system/parser.js");
include( dir+"system/compiler.js");
include( dir+"system/pause.js");
include( dir+"system/call.js");
include( dir+"system/interpreter.js");
include( dir+"library/infra.js"); //< problem: global vars declared with 'var' are not kept!
// will fix with google closure by compiling all the files in to one.
include( dir+"library/r6rs_lib.js");
include( dir+"library/js_interface.js");
include( dir+"library/webscheme_lib.js");
include( dir+"library/extra_lib.js");
include( dir+"library/srfi.js");
if( WKIT ) include( dir+"platforms/browser/dumper.js");
*/
