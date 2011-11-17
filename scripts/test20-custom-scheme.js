try {

var include = Loco.ctx.include,
    print = Loco.console.println,
    readline = Loco.console.readLine,
    fread = Loco.ctx.read,
    exit = Loco.ctx.exit,
    ctx = Loco.ctx, 
    WEBKIT = Loco.ctx.jsInterpreterName().indexOf( "webkit" ) >= 0, 
    err = function(msg) { throw msg; }

Loco.ctx.onError.connect( function( msg ) { print( msg ); } );


//==============================================================================
if( !WEBKIT ) err( "NEED WEBKIT" );  
var reqHandler = ctx.create( "NetworkRequestHandler" );
ctx.setEnableCustomNetRequestHandlers( true );
ctx.addNetworkRequestHandler( "myscheme", reqHandler );
function handleRequest( req, reply ) {
  var content = "<head>\n \
  <title>Custom scheme test</title>\n \
  </head>\n  \
  <body>\n \
  <h1>Custom scheme test</h1> \
  URL: <pre>@1</pre> <br/> \
  SCHEME: <pre>@2</pre> </br>\
  PARAMETERS: <pre>@3</pre> </br> \
  This page comes from a custom scheme(protocol) handler\n \
  <br/><a href='javascript:webWindow.close()'>close</a>\n \
  </body>\n \
  </html>".replace(/@1/,req.url)
          .replace(/@2/,req.scheme)
          .replace(/@3/,(function(){
                         var q = "<br/>";
                         for( var i in req.query ) {
                           q += i + ":";
                           for( var v = 0; v != req.query[ i ].length; ++v ) {
                             q += " " + req.query[ i ][ v ];
                           }
                           q += "<br/>";
                         }
                         return q; })());
  reply.setHeader( "ContentType", "text/html; charset ASCII" );
  reply.setHeader( "ContentLength", content.length );
  reply.setContent( content );
  reply.setUrl( req.url );
}
reqHandler.handleRequest.connect( handleRequest );
var ww = Loco.gui.create( "WebWindow" );
//allow references to window from code executed within the window
ww.addObjectToContext( ww, "webWindow" ); 
ww.onError.connect( err );
if( !ww.syncLoad( "./test20-custom-scheme.html", 2000  ) ) throw "ERROR LOADING PAGE";
ww.show();

//==============================================================================

//exit(0);

} catch( e ) {
  if( e.message ) Loco.console.printerrln( e.message );
  else Loco.console.printerrln( e );
  exit( -1 );
}    
