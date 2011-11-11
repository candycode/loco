#!/bin/sh
JSFILES_NO_WEB="--js version.js  --js deps/underscore.js --js deps/underscore.string.js \
--js system/class.js --js stackbase.js \
--js system/set.js --js system/write.js --js system/pair.js --js system/value.js \
--js system/symbol.js --js system/char.js --js system/number.js --js system/port.js \
--js system/record.js --js system/hashtable.js --js system/syntax.js --js system/types.js \
--js system/parser.js --js system/compiler.js --js system/pause.js --js system/call.js --js system/interpreter.js \
--js library/infra.js --js library/r6rs_lib.js --js library/js_interface.js --js library/webscheme_lib.js \
--js library/extra_lib.js --js library/srfi.js"
JSFILES_WEB=" --js deps/jquery.js --js platforms/browser/console.js --js platforms/browser/dumper.js"
#COMPILER="/c/Programs/closure/compiler.jar"
COMPILER="$HOME/apps/closure/compiler.jar"
java -jar $COMPILER $JSFILES_NO_WEB --third_party --compilation_level WHITESPACE_ONLY --js_output_file biwascheme_no_browser_closure_compiled.js
java -jar $COMPILER $JSFILES_NO_WEB $JSFILES_WEB --third_party --compilation_level WHITESPACE_ONLY --js_output_file biwascheme_closure_compiled.js
