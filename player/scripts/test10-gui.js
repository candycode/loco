try {
// prelude
  var $include = Loco.ctx.include;
  var print = Loco.console.println;
  var ctx = Loco.ctx;

  $include( 'keys.js' );
 

// function invoked in web window when trance requested  
  function showTrace() {
    function format( e, i ) {
      var f = "<ul><li><em>name</em>: ";
      if( e["name"] === undefined || e["name"] === null ) f += "anonymous";
      else  f += e["name"];
      f +="</li><li><em>execution time</em>: ";
      if( i < Wrapper.log.length - 1 ) f +=  Wrapper.log[ i + 1 ]["time"] - e["time"] + " ms";
      else f +=  "N/A";
      f += "</li><li><em>arguments</em>: ";
      var args = e["args"];
      for( var a = 0; a < args.length; a += 1 ) {
        f += args[ a ];
        if( a !== args.length ) f += ", ";
      }
      f += "</li><li><em>return</em>: ";
      if( args["return"] === undefined || args["return"] === null ) f += "void";
      else f += args["return"];
      f += "</li></ul>";
      return f;
    };
    var w = Loco.gui.create('WebMainWindow');
    var html = "<html><head></head><body style='font-family:\"Courier new\"'><ol>";
    var entries = Wrapper.log;
    for( var e = 0; e < Wrapper.log.length; e += 1 ) {
      html += "<li>" + format( entries[ e ], e ) + "</li>";
    }
    html += "</ol></body></html>";
    w.setHtml( html );
    w.show();
  }

// main window menu
  var menu = {
    "Log" : {
      "Show trace": {
          "cback"  : showTrace.toString() + "showTrace();",
          "tooltip": "Show trace",
          "status" : "Show call trace",
          "icon"   : ""
      }
    },
    "Exit" : {
      "Quit": {
          "cback"  : "Loco.ctx.quit()",
          "tooltip": "Quit",
          "status" : "Exit from application",
          "icon"   : ""
       }
     }
    };
 
// command line
  var cmdParam = ctx.cmdLine()[ctx.cmdLine().length - 1];
  var WEBSITE = cmdParam.lastIndexOf( ".js" ) < 0 ? 
                cmdParam : "http://www.nyt.com";  

// create main window  
  var ww = Loco.gui.create( "WebMainWindow" );
  ww.setName( "webWindow" ); 
  ww.addParentObjectsToJS(); // add objects in this context to web page context
  ww.addSelfToJSContext();   // add web page to its own context

// code to add tracing wrappers to every global javascript function 
  var jsFilter = 'var Wrapper = { wrap: function(that, v) { \
                    var f = that[ v ]; \
                    var thisObj = this; \
                    return function() { \
                      var ret = f.apply( null, arguments ); \
                      thisObj.log.push( { "time": new Date().getTime(), "name": v, "args": arguments, "return": ret } ); \
                      return ret; \
                    } \
                  }, \
                  log: [] \
                 };';
  var wrap = 'for( v in this ) { \
                try { \
                  if( typeof this[ v ] === "function" ) { \
                    this[ v ] = Wrapper.wrap( this, v ); \
                  } \
                } catch( e ) { \
                  Loco.console.println( "Cannot wrap " + v + " -- " + e ); \
                } \
              }'; 

 // setup main window 
  ww.setMenu( menu );
  ww.setAttributes( {DeveloperExtrasEnabled: true } );
  ww.setEnableContextMenu( true );
  ww.loadProgress.connect( function( i ) { ww.setStatusBarText( i + "%" ); } );
  ww.keyPress.connect( function( k, m, c ) { 
                         if( k === LocoKey.F11 ) ww.showNormal();
                       } );
  ww.setPreLoadCBack( jsFilter+wrap );
  ww.loadFinished.connect( function( ok ) { 
    if( ok ) ww.eval("Loco.webWindow.setStatusBarText('DONE');");
    else Loco.gui.errorDialog( "Error loading page" );
  });
  ww.setStatusBarText( "Loading..." );
  ww.setWindowTitle( ctx.appName() ); 
  ctx.onError.connect( function( err ) { Loco.gui.criticalDialog( "Error", err ); ctx.exit( -1 ); } );
  ww.show(); 
  ww.load(WEBSITE);
  
} catch(e) {
  Loco.console.printerrln(e);
  Loco.ctx.exit( -1 );
}

