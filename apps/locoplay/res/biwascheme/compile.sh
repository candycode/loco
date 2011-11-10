#!/bin/sh
set JSFILES="--js version.js --js stackbase.js \
--js deps/jquery.js --js deps/underscore.js --js deps/underscore.string.js \
--js library/extra_lib.js --js library/infra.js --js library/js_interface.js \
--js library/r6rs_lib.js --js library/srfi.js --js library/webscheme_lib.js
--js platforms/browser/console.js --js platforms/browser/dumper.js \
--js system/set.js --js system/write.js --js system/pair.js --js system/value.js \
--js system/symbol.js --js system/char.js --js system/number.js --js system/port.js \
--js system/record.js --js system/hashtable.js --js system/syntax.js --js system/types.js \
--js system/parser.js --js system/compiler.js --js system/pause.js --js system/call.js --js system/interpreter.js"
echo $JSFILES
set COMPILER=/c/Programs/closure/compiler.jar
java -jar $COMPILER $JSFILES --js_output_file closure_compiled.js