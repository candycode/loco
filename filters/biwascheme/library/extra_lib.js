
if( typeof(BiwaScheme)!='object' ) BiwaScheme={}; with(BiwaScheme) {
  define_libfunc("html-escape", 1, 1, function(ar){
    assert_string(ar[0]);
    return _.escapeHTML(ar[0]);
  });
  BiwaScheme.inspect_objs = function(objs){
    return _.map(objs, BiwaScheme.inspect).join(", ");
  };
  define_libfunc("inspect", 1, null, function(ar){
    return BiwaScheme.inspect_objs(ar);
  });
  define_libfunc("inspect!", 1, null, function(ar){
    puts(BiwaScheme.inspect_objs(ar));
    return BiwaScheme.undef;
  });

  //
  // json
  //
  // json->sexp
  // Array -> list
  // Object -> alist
  // (number, boolean, string, 
  //
  BiwaScheme.json2sexp = function(json){
    switch(true){
    case _.isNumber(json) ||
         _.isString(json) ||
         json === true || json === false:
      return json;
    case _.isArray(json):
      return array_to_list(_.map(json, json2sexp));
    case typeof(json) == "object":
      var ls = nil;
      for(key in json){
        ls = new Pair(new Pair(key, json2sexp(json[key])),
               ls);
      }
      return ls;
    default:
      throw new Error("json->sexp: detected invalid value for json: "+BiwaScheme.inspect(json));
    }
    throw new Bug("must not happen");
  }
  define_libfunc("json->sexp", 1, 1, function(ar){
    return json2sexp(ar[0]);
  })

  //
  //from Gauche
  //

  define_libfunc("identity", 1, 1, function(ar){
    return ar[0];
  });

  // string
  
  define_libfunc("string-concat", 1, 1, function(ar){
    assert_list(ar[0]);
    return ar[0].to_array().join("");
  })

  define_libfunc("string-split", 2, 2, function(ar){
    assert_string(ar[0]);
    assert_string(ar[1]);
    return array_to_list(ar[0].split(ar[1]));
  })

  define_libfunc("string-join", 1, 2, function(ar){
    assert_list(ar[0]);
    var delim = ""
    if(ar[1]){
      assert_string(ar[1]);
      delim = ar[1];
    }
    return ar[0].to_array().join(delim);
  })
  
  // lists

  define_libfunc("intersperse", 2, 2, function(ar){
    var item = ar[0], ls = ar[1];
    assert_list(ls);

    var ret = [];
    _.each(ls.to_array().reverse(),function(x){
      ret.push(x);
      ret.push(item);
    });
    ret.pop();
    return array_to_list(ret);
  });

  define_libfunc("map-with-index", 2, null, function(ar){
    var proc = ar.shift(), lists = ar;
    _.each(lists, assert_list);

    var results = [], i = 0;
    return Call.multi_foreach(lists, {
      call: function(xs){ 
        var args = _.map(xs, function(x){ return x.car });
        args.unshift(i);
        i++;
        return new Call(proc, args);
      },
      result: function(res){ results.push(res); },
      finish: function(){ return array_to_list(results); }
    });
  });

  // sorting
  
  // These functions takes a Scheme proc and sort the given
  // list or vector using the proc as the compare function.
  //
  // Limitations:
  //  - you should not use Ajax or sleep inside the proc
  //  - you cannot access to the free variables outside the proc
  //
  // (list-sort/comp proc list)
  // (vector-sort/comp proc vector)
  // (vector-sort/comp! proc vector)

  // utility function. takes a JS Array and a Scheme procedure,
  // returns sorted array
  var sort_with_comp = function(ary, proc){
    return ary.sort(function(a, b){
        var intp2 = new BiwaScheme.Interpreter();
        return intp2.invoke_closure(proc, [a, b]);
      });
  };

  define_libfunc("list-sort/comp", 1, 2, function(ar){
    assert_procedure(ar[0]);
    assert_list(ar[1]);

    return array_to_list(sort_with_comp(ar[1].to_array(), ar[0]));
  });
  define_libfunc("vector-sort/comp", 1, 2, function(ar){
    assert_procedure(ar[0]);
    assert_vector(ar[1]);

    return sort_with_comp(_.clone(ar[1]), ar[0]);
  });
  define_libfunc("vector-sort/comp!", 1, 2, function(ar){
    assert_procedure(ar[0]);
    assert_vector(ar[1]);

    sort_with_comp(ar[1], ar[0]);
    return BiwaScheme.undef;
  });
  
  // macros

  //(define-macro (foo x) body ...)
  //(define-macro foo lambda)

  var rearrange_args = function (expected, given) {
    var args = [];
    var dotpos = (new Compiler).find_dot_pos(expected);
    if (dotpos == -1)
      args = given;
    else {
      for (var i = 0; i < dotpos; i++) {
        args[i] = given[i];
      }
      args[i] = array_to_list(given.slice(i));
    }
    return args;
  }
  define_syntax("define-macro", function(x){
    var head = x.cdr.car;
    var expected_args;
    if(head instanceof Pair){
      var name = head.car;
      expected_args = head.cdr;
      var body = x.cdr.cdr;
      var lambda = new Pair(Sym("lambda"),
                     new Pair(expected_args,
                       body))
    }
    else{
      var name = head;
      var lambda = x.cdr.cdr.car;
      expected_args = lambda.cdr.car;
    }

    //[close, n_frees, do_body, next]
    var opc = Compiler.compile(lambda);
    if(opc[1] != 0)
      throw new Bug("you cannot use free variables in macro expander (or define-macro must be on toplevel)")
    var cls = [opc[2]];

    TopEnv[name.name] = new Syntax(name.name, function(sexp){
      var given_args = sexp.to_array();

      given_args.shift();
      
      var intp = new Interpreter();
      var args = rearrange_args(expected_args, given_args);
      var result = intp.invoke_closure(cls, args);
      return result;
    });

    return BiwaScheme.undef;
  })

  var macroexpand_1 = function(x){
    if(x instanceof Pair){
      if(x.car instanceof Symbol && TopEnv[x.car.name] instanceof Syntax){
        var transformer = TopEnv[x.car.name];
        x = transformer.transform(x);
      }
      else
        throw new Error("macroexpand-1: `" + to_write_ss(x) + "' is not a macro");
    }
    return x;
  }
  define_syntax("%macroexpand", function(x){
    var expanded = (new Interpreter).expand(x.cdr.car);
    return List(Sym("quote"), expanded);
  });
  define_syntax("%macroexpand-1", function(x){
    var expanded = macroexpand_1(x.cdr.car);
    return List(Sym("quote"), expanded);
  });

  define_libfunc("macroexpand", 1, 1, function(ar){
    return (new Interpreter).expand(ar[0]);
  });
  define_libfunc("macroexpand-1", 1, 1, function(ar){
    return macroexpand_1(ar[0]);
  });

  define_libfunc("gensym", 0, 0, function(ar){
    return BiwaScheme.gensym();
  });
  
  // i/o

  define_libfunc("print", 1, null, function(ar){
    _.map(ar, function(item){
      puts(to_display(item), true);
    })
    puts(""); //newline
    return BiwaScheme.undef;
  })
  define_libfunc("write-to-string", 1, 1, function(ar){
    return to_write(ar[0]);
  });
  define_libfunc("read-from-string", 1, 1, function(ar){
    assert_string(ar[0]);
    return Interpreter.read(ar[0]);
  });
  define_libfunc("port-closed?", 1, 1, function(ar){
    assert_port(ar[0]);
    return !(ar[0].is_open);
  });
  
  // syntax
  
  define_syntax("let1", function(x){
    //(let1 vari expr body ...) 
    //=> ((lambda (var) body ...) expr)
    var vari = x.cdr.car; 
    var expr = x.cdr.cdr.car;
    var body = x.cdr.cdr.cdr;

    return new Pair(new Pair(Sym("lambda"),
                      new Pair(new Pair(vari, nil),
                        body)),
             new Pair(expr, nil));
  })

  //
  // Regular Expression
  //
  var assert_regexp = function(obj, fname){
    if(!(obj instanceof RegExp))
      throw new Error(fname + ": regexp required, but got " + to_write(obj));
  }

  //Function: string->regexp string &keyword case-fold 
  define_libfunc("string->regexp", 1, 1, function(ar){
    assert_string(ar[0], "string->regexp");
    return new RegExp(ar[0]); //todo: case-fold
  })
  //Function: regexp? obj 
  define_libfunc("regexp?", 1, 1, function(ar){
    return (ar[0] instanceof RegExp);
  })
  //Function: regexp->string regexp 
  define_libfunc("regexp->string", 1, 1, function(ar){
    assert_regexp(ar[0], "regexp->string");
    return ar[0].toString().slice(1, -1); //cut '/' 
  })

  define_libfunc("regexp-exec", 2, 2, function(ar){
    var rexp = ar[0];
    if(_.isString(ar[0])){
      rexp = new RegExp(ar[0]);
    }
    assert_regexp(rexp, "regexp-exec");
    assert_string(ar[1], "regexp-exec");
    var ret = rexp.exec(ar[1])
    return (ret === null) ? false : array_to_list(ret);
  })

//  //Function: rxmatch regexp string 
//  define_libfunc("rxmatch", 1, 1, function(ar){
//    assert_regexp(ar[0], "rxmatch");
//    assert_string(ar[1], "rxmatch");
//    return ar[0].match(ar[1]);
//  });
  //Function: rxmatch-start match &optional (i 0) 
  //Function: rxmatch-end match &optional (i 0) 
  //Function: rxmatch-substring match &optional (i 0) 
  //Function: rxmatch-num-matches match   
  //Function: rxmatch-after match &optional (i 0) 
  //Function: rxmatch-before match &optional (i 0) 
  //Generic application: regmatch &optional index 
  //Generic application: regmatch 'before &optional index 
  //Generic application: regmatch 'after &optional index 
  //Function: regexp-replace regexp string substitution 
  //Function: regexp-replace-all regexp string substitution 
  //Function: regexp-replace* string rx1 sub1 rx2 sub2 ... 
  //Function: regexp-replace-all* string rx1 sub1 rx2 sub2 ... 
  //Function: regexp-quote string 
  //Macro: rxmatch-let match-expr (var ...) form ... 
  //Macro: rxmatch-if match-expr (var ...) then-form else-form 
  //Macro: rxmatch-cond clause ... 
  //Macro: rxmatch-case string-expr clause ... 

}


