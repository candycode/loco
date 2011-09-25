try {

  var $include = Loco.ctx.include;
  var print = Loco.console.println;
  var ctx = Loco.ctx;

  $include( 'keys.js' );
 
  
  function showTrace() {
    function format( e, i ) {
      var f = "<ul><li><em>name</em>: ";
      if( e["name"] === undefined || e["name"] === null ) f += "anonymous";
      else  f += e["name"];
      f +="</li><li><em>execution time</em>: ";
      if( i < Wrapper.log.length - 1 ) f +=  Wrapper.log[ i + 1 ]["time"] - e["time"] + " ms";
      else f +=  "N/A";//e["time"] - timeFinished  + " ms";
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

  var menu = {
    "Log" : {
      "Show trace": {
          "cback"  : showTrace.toString() + "showTrace();",
          "tooltip": "Show trace",
          "status" : "Show call trace"
      }
    },
    "Exit" : {
      "Quit": {
          "cback"  : "Loco.ctx.quit()",
          "tooltip": "Quit",
          "status" : "Exit from application"
       }
     }
    };
 
  
  var ww = Loco.gui.create( "WebMainWindow" );
  ww.addParentObjectsToJS();
  ww.addSelfToJSContext( "webWindow" );

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
                Loco.console.println( "[X] " + v + " -- " + e ); \
                } \
              }'; 
   
  ww.setMenu( menu );
  ww.setAttributes( {DeveloperExtrasEnabled: true } );
  ww.setEnableContextMenu( true );
  ww.loadProgress.connect( function( i ) { ww.setStatusBarText( i + "%" ); } );
  ww.keyPress.connect( function( k, m, c ) { 
                         if( k === LocoKey.F11 ) ww.showNormal();
                       } );
  //ww.loadStarted.connect( function() { print("load started"); ww.eval(jsFilter+wrap); } );
  ww.setPreLoadCBack( jsFilter+wrap );
  //ww.loadFinished.connect( function() { ww.eval("var timeFinished = new Date().getTime()"); });// webWindow.setStatusBarText('DONE');"); } );
  ww.setStatusBarText( "Loading..." );
  ww.setWindowTitle( ctx.appName() ); 
  ctx.onError.connect( function( err ) { Loco.gui.criticalDialog( "Error", err ); ctx.exit( -1 ); } );
  ww.show(); 
  ww.load("http://www.nyt.com");
  
} catch(e) {
  Loco.console.printerrln(e);
  Loco.ctx.exit( -1 );
}

