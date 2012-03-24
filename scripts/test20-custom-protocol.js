try {

var include = Loco.ctx.include,
    print = Loco.console.println,
    readline = Loco.console.readLine,
    fread = Loco.fs.fread,
    exit = Loco.ctx.exit,
    ctx = Loco.ctx,
    quit = Loco.ctx.quit,
    HOMEDIR = Loco.ctx.homeDir(),
    APPDIR = Loco.ctx.appDir(),
    CURDIR = Loco.ctx.curDir(), 
    WEBKIT = Loco.ctx.jsInterpreterName().indexOf( "webkit" ) >= 0, 
    err = function(msg) { throw msg; }

Loco.ctx.onError.connect( function( msg ) { print( msg ); } );
//==============================================================================
if( !WEBKIT ) err( "NEED WEBKIT" );  
var customReqHandler = ctx.create( "ProtocolHandler" );
ctx.setEnableCustomProtocolHandlers( true );
ctx.addProtocolHandler( "myprotocol", customReqHandler );
function handleCustomRequest( req, reply ) {
  var content = 
 "<head>\n \
  <title>Custom protocol test</title>\n \
  </head>\n  \
  <body>\n \
  <h1>Custom protocol test</h1> \
  URL: <pre>@1</pre> <br/> \
  PROTOCOL(SCHEME): <pre>@2</pre> </br>\
  PARAMETERS: <pre>@3</pre> </br> \
  This page comes from a custom protocol handler\n \
  <br/><a href='/?p1=23&p2=54&p3=55'>invoke again with different parameters</a>\n \
  <!--WARNING the baseURI is set to 'myscheme://...', to load pages through other protocols\n \
      explicitly invoke the load method or pass the url to one of the parameters of the custom\n \
      request and properly handle it in the protocol handling function\n \
  --> \
  <br/><a href='javascript:webWindow.load(\"file://@CURDIR/test20-custom-protocol.html\")'>back</a>\n \
  <br/><br/><a href='javascript:webWindow.close()'>close</a>\n \
  </body>\n \
  </html>".replace(/@CURDIR/, CURDIR )
          .replace(/@1/,req.url.replace(/&/g, "&#38;"))
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
  reply.setUrl( "file://" ); 
}
customReqHandler.handleRequest.connect( handleCustomRequest );
var ww = Loco.gui.create( "WebWindow" );
ww.setAttributes( {JavascriptEnabled: true,
                   DeveloperExtrasEnabled: true,
                   LocalContentCanAccessFileUrls: true} );
ww.setEnableContextMenu( true );
//allow references to window from code executed within the window
ww.addObjectToContext( ww, "webWindow" ); 
ww.onError.connect( err );
var page =  "file://" + CURDIR +"/test20-custom-protocol.html";
ww.load( page );
ww.show();
//==============================================================================
} catch( e ) {
  if( e.message ) Loco.console.printerrln( e.message );
  else Loco.console.printerrln( e );
  exit( -1 );
}    
