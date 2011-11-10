///
/// infra.js - Basis for library functions
///

//
// define_*func - define library functions
//
BiwaScheme.check_arity = function(len, min, max){
  var fname = arguments.callee.caller
                ? arguments.callee.caller.fname
                : "(?)";
  if(len < min){
    if(max && max == min)
      throw new BiwaScheme.Error(fname+": wrong number of arguments (expected: "+min+" got: "+len+")");
    else
      throw new BiwaScheme.Error(fname+": too few arguments (at least: "+min+" got: "+len+")");
  }
  else if(max && max < len)
    throw new BiwaScheme.Error(fname+": too many arguments (at most: "+max+" got: "+len+")");
}
BiwaScheme.define_libfunc = function(fname, min, max, func, is_raw){
  var f = function(ar, intp){
    BiwaScheme.check_arity(ar.length, min, max);
    var result = func(ar, intp);
    if (is_raw) {
      return result;
    }
    else{
      if (result === undefined){
        throw new BiwaScheme.Bug("library function " + 
                                 "`" + fname + "'" +
                                 " returned JavaScript's undefined");
      }
      else if (result === null){
        throw new BiwaScheme.Bug("library function " +
                                 "`" + fname + "'" + 
                                 " returned JavaScript's null");
      }
      else {
        return result;
      }
    }
  };

  func["fname"] = fname; // for assert_*
  f["fname"]    = fname; // for check_arity
  f["inspect"] = function(){ return this.fname; }
  BiwaScheme.CoreEnv[fname] = f;
}
BiwaScheme.alias_libfunc = function(fname, aliases) {
  if (BiwaScheme.CoreEnv[fname]) {
    if (_.isArray(aliases)) {
      _.map(aliases, function(a) { BiwaScheme.alias_libfunc(fname, a); });
    } else if (_.isString(aliases)) {
      BiwaScheme.CoreEnv[aliases] = BiwaScheme.CoreEnv[fname];
    } else {
      throw new BiwaScheme.Bug("bad alias for library function " +
                               "`" + fname + "': " + aliases.toString());
    }
  } else {
    throw new BiwaScheme.Bug("library function " +
                             "`" + fname + "'" +
                             " does not exist, so can't alias it.");
  }
};
BiwaScheme.define_libfunc_raw = function(fname, min, max, func){
  BiwaScheme.define_libfunc(fname, min, max, func, true);
}
BiwaScheme.define_syntax = function(sname, func) {
  var s = new BiwaScheme.Syntax(sname, func);
  BiwaScheme.TopEnv[sname] = s;
}
BiwaScheme.define_scmfunc = function(fname, min, max, str){
  (new Interpreter).evaluate("(define "+fname+" "+str+"\n)");
}

//  define_scmfunc("map+", 2, null, 
//    "(lambda (proc ls) (if (null? ls) ls (cons (proc (car ls)) (map proc (cdr ls)))))");

//
// assertions - type checks
//
make_assert = function(check){
  return function(/*args*/){
    var fname = arguments.callee.caller
                  ? arguments.callee.caller.fname 
                  : "";
    check.apply(this, [fname].concat(_.toArray(arguments)));
  }
}
make_simple_assert = function(type, test){
  return make_assert(function(fname, obj, opt){
    option = opt ? ("("+opt+")") : ""
    if(!test(obj)){
      throw new BiwaScheme.Error(fname + option + ": " +
                                 type + " required, but got " +
                                 BiwaScheme.to_write(obj));
    }
  })
}

assert_number = make_simple_assert("number", function(obj){
  return typeof(obj) == 'number' || (obj instanceof BiwaScheme.Complex);
});

assert_integer = make_simple_assert("integer", function(obj){
  return typeof(obj) == 'number' && (obj % 1 == 0)
});

assert_real = make_simple_assert("real number", function(obj){
  return typeof(obj) == 'number';
});

assert_between = make_assert(function(fname, obj, from, to){
  if( typeof(obj) != 'number' || obj != Math.round(obj) ){
    throw new BiwaScheme.Error(fname + ": " +
                               "number required, but got " +
                               BiwaScheme.to_write(obj));
  }

  if( obj < from || to < obj ){
    throw new BiwaScheme.Error(fname + ": " + 
                               "number must be between " + 
                               from + " and " + to + ", but got " +
                               BiwaScheme.to_write(obj));
  }
});

assert_string = make_simple_assert("string", _.isString);

assert_char = make_simple_assert("character", BiwaScheme.isChar);
assert_symbol = make_simple_assert("symbol", BiwaScheme.isSymbol);
assert_port = make_simple_assert("port", BiwaScheme.isPort);
assert_pair = make_simple_assert("pair", BiwaScheme.isPair);
assert_list = make_simple_assert("list", BiwaScheme.isList);
assert_vector = make_simple_assert("vector", BiwaScheme.isVector);

assert_hashtable = make_simple_assert("hashtable",
                                          BiwaScheme.isHashtable);
assert_mutable_hashtable = make_simple_assert("mutable hashtable", 
                                            BiwaScheme.isMutableHashtable);

assert_record = make_simple_assert("record",
                                          BiwaScheme.isRecord);
assert_record_td = make_simple_assert("record type descriptor",
                                          BiwaScheme.isRecordTD);
assert_record_cd = make_simple_assert("record constructor descriptor",
                                          BiwaScheme.isRecordCD);

assert_function = make_simple_assert("JavaScript function", 
                                         _.isFunction);
assert_closure = make_simple_assert("scheme function", 
                                        BiwaScheme.isClosure);
assert_procedure = make_simple_assert("scheme/js function", function(obj){
  return BiwaScheme.isClosure(obj) || _.isFunction(obj);
});

assert_date = make_simple_assert("date", function(obj){
  // FIXME: this is not accurate (about cross-frame issue)
  // https://prototype.lighthouseapp.com/projects/8886/tickets/443
  return obj instanceof Date;
});

//var assert_instance_of = make_assert(function(fname, type, obj, klass){
//  if(!(obj instanceof klass)){
//    throw new BiwaScheme.Error(fname + ": " +
//                               type + " required, but got " +
//                               BiwaScheme.to_write(obj));
//  }
//});

assert = make_assert(function(fname, success, message){
  if(!success){
    throw new BiwaScheme.Error(fname+": "+message);
  }
});


