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
   
    var html = "<html><head></head><body style='font-family:\"Courier new\"'><ol>";
    var entries = Wrapper.log;
    for( var e = 0; e < Wrapper.log.length; e += 1 ) {
      html += "<li>" + format( entries[ e ], e ) + "</li>";
    }
    html += "</ol></body></html>";
    var sw = gui.create( "WebWindow" );
    if( Loco.wctx.os() !== "MAC")  {
      sw.setParentWindow( mainWindow );
      sw.setWindowStyle( ["dialog"] );
    } else {
      sw.setParentWindow( mainWindow );
      sw.setWindowStyle( ["drawer"] );
    }  
    sw.setHtml( html );
    sw.show();
  }

// main window menu
  var menu = {
    "Log" : {
      "Show trace": {
          "cback"  : 'ww.eval( "showTrace()" )',
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
  var ww = Loco.gui.create( "WebWindow" );
  ww.addObjectToContext( ww, "webWindow" );
  ww.addObjectToContext( Loco.gui, "gui" );
 
 
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
                 };\n';
  var wrap = 'for( v in this ) { \
                try { \
                  if( typeof this[ v ] === "function" ) { \
                    this[ v ] = Wrapper.wrap( this, v ); \
                  } \
                } catch( e ) { \
                  Loco.console.println( "Cannot wrap " + v + " -- " + e ); \
                } \
              };\n'; 

 // setup main window 
  var mw = Loco.gui.create( "MainWindow" );
  //mw.setMask( "./mask.png" ); 
  //mw.setWindowOpacity( 0.3 );
  mw.setMenu( menu );
  ww.addObjectToContext( mw, "mainWindow" );
  mw.setCentralWidget( ww );

  ww.setAttributes( {DeveloperExtrasEnabled: true,
                     LocalContentCanAccessFileUrls: true,
                     LocalContentCanAccessRemoteUrls: true } );
  ww.setEnableContextMenu( true );
  ww.loadProgress.connect( function( i ) { mw.setStatusBarText( i + "%" ); } );
  ww.keyPress.connect( function( k, m, c ) { 
                         if( k === LocoKey.F11 ) ww.showNormal();
                       } );
  ww.setPreLoadCBack( jsFilter + wrap + showTrace.toString() );
  ww.loadFinished.connect( function( ok ) { 
    if( ok ) mw.setStatusBarText( 'DONE' );
    else Loco.gui.errorDialog( "Error loading page" );
  });
  mw.setStatusBarText( "Loading..." );
  mw.setWindowTitle( ctx.appName() ); 
  ctx.onError.connect( function( err ) { Loco.gui.criticalDialog( "Error", err ); ctx.exit( -1 ); } );
  mw.show(); 
  ww.load(WEBSITE);
  
} catch(e) {
  Loco.console.printerrln(e);
  Loco.ctx.exit( -1 );
}

