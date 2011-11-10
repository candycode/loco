try {
var dir = "../filters/biwascheme/";
var include = Loco.ctx.include;

var BiwaScheme = BiwaScheme || {};

var print = Loco.console.print;
function puts( str, no_newline )  {
  print(str);
  if (!no_newline) {
    print("\n");
  }
}

var Console = {};

Console.puts = function(str, no_newline) {
  print(str);
  if (!no_newline) {
    print("\n");
  }
};

Console.p = function() {
  print.apply(this, arguments);
};

var WKIT = Loco.ctx.jsInterpreterName().indexOf( "webkit" ) >= 0;

include( dir+"version.js");
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

if( WKIT ) {
  BiwaScheme.Interpreter.dumper = new BiwaScheme.Dumper();
}

function show_error(e){
  puts("Error: "+e.message);
}
function ev(str){
  puts("ev> "+str);
  var ret = (new BiwaScheme.Interpreter(show_error)).evaluate(str);
  return ret;
}

var E = ev( "(+ 1 2 )" );
ev( '(js-eval "Loco.console.println( \'ciao\' )")' );
puts( typeof E );
puts( E );
Loco.ctx.exit( 0 );
} catch(e) { Loco.console.println(e); }
