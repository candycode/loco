---
layout: default
analytics: true
---

##LoCO - Loosely Coupled Objects

_LoCO_ is a set of _Qt_ _C++_ classes that make it easy to create command-line and _GUI_ applications
 with any language that compiles to _JavaScript_.

_JavaScript_ is used to glue together binary components, optionally loaded at run-time,
developed in _C++_. 

Objects are connected through signals/slots or by direct reference through a _QObject_ pointer,
giving to the objects the responsibility to check the interface semantics or the object type.

##How does it look like ?

Say you want to create an _OpenGL_-based application with a _HUD_-like _GUI_, here
is how you do it.

**JavaScript**

1. load the _OpenGL_ application/module
2. create a _GUI_ with a window and an overlaid transparent html page
   acting as a _HUD_ interface
3. route events
4. open window

The _OpenGL_ app is stored into a binary run-time loadable object and knows
nothing about the surrounding environment(i.e. _LoCO_); it simply exposes invokabe
_QObject_ slots or _Q_INVOKABLE_ methods.

{% highlight javascript %}

var glapp = Loco.ctx.loadQtPlugin( Loco.ctx.args[ 2 ] );
var glformat = glapp.glFormat ? glapp.glFormat : { alpha: true, depth: true };
var view = Loco.gui.create( "GraphicsView" );
view.initGL.connect( glapp.initGL );
view.createOpenGLViewport( glformat );
view.createGraphicsSceneProxy();
var ww = Loco.gui.create( "GraphicsWebWindow" );
ww.addObjectToContext( glapp, "app" );
// add GUI object to access e.g. native dialogs
ww.addObjectToContext( Loco.gui, "gui" );
ww.setTransparent();
ww.syncLoad( "./test33-overlay.html" );
view.show( 50, 50, 800, 600 );

{% endhighlight %}


**C++ application**

The application is implemented as a stand-alone dynamically loadable object
which exposes a set of methods invoked from the run-time environment.
There is no requirement for the application to know anything about the _LoCO_
environment; a set of classes is however provided to help with sharing
common information with the _LoCO_ run-time and other loaded modules should the
need arise.  

{% highlight cpp %}

class GLApp : public QObject, ... {
Q_OBJECT
        Q_PROPERTY( QVariantMap bkcolor READ GetBkColor 
                                        WRITE SetBkColor )
...

public slots:
    void initGL() {
        GLenum status = glewInit();
        if( status != GLEW_OK ) {
            emit error( QString( "GLEW initialization failed" ) + 
                        QString( (char*) glewGetErrorString( status ) ) );
            return;
        }

        ... 

{% endhighlight %}

**C++ driver application**

_LoCO_ is a library. You need to create an application which uses such
library to configure and bootstrap the _JavaScript_ environment.

Many options are available for creating sandboxed environment with
access restriction on local files and network resources.

A sample application(total source size 6.9kb), [_locoplay_](https://github.com/candycode/loco/tree/master/apps/locoplay), is provided; you can just take it and taylor it to suit your needs, although
in most cases you can simply use it as it is and simply change the compile-time configuration 
flags as needed.

_excerpt from locoplay_:

{% highlight cpp %}

...
LocoQtApp qtApp( argc, argv );
loco::App app( qtApp, argc, argv, i ); 
#ifdef LOCOPLAY_WKIT
        app.SetInterpreter( new WebKitJSCore ); 
#else
        app.SetInterpreter( new QScriptInterpreter );
#endif
#ifdef LOCOPLAY_FSYSTEM
        app.AddModuleToJS( new FileSystem );
        app.SetAllowFileAccess( true );
        app.SetFilterFileAccess( false );
#endif
...
app.InitContext();
...
app.ParseCommandLine();
ret = app.Execute();

{% endhighlight %} 

There is an ovverridable naming convention to map _loco::Object_ derived C++ objects 
to JavaScript ones; by default all the exposed objects(filesystem, context, console...)
are accessed from JavaScript through the global _Loco_ object.

##History and status

This project started several years ago when I got tired of spending time writing _C_
and _C++_ code to build _MVP/MVVM/MVC_ application logic and binding UI events to callbacks with 
_MOTIF/MFC/GTK/Qt/WPF_... + a few mobile frameworks. After some time spent experimenting 
with different scripting languages and their bindings to _GUI_ frameworks I settled on 
_Qt_ for desktop applications simply because it has been and still is the fastest path 
to building cross-platform applications scriptable in a widespread scripting 
language such as _ECMAScript/JavaScript_.

If I had the bandwidth I would take _V8_ and build wrappers around native *GUI*s(with
declarative support through **standard** _JSON_ ), _WebKit_, filesystem, network and multimedia
libraries with the addition of a resource management system and _i18n_ support.
Since however I cannot afford to do that I am using _Qt_.

###Status

The code you see here was physically extracted from a private project hosted on _GitHub_, 
so the _GitHub_ project you see is now what it used to be my private development branch and it is 
therefore a work in progress.

The current project is a stripped down, cleaned-up, partially rewritten version of a larger and much 
garbled project which also had some _Lua_, _Python_, _Tcl_ and _Scheme_ bindings; the only additional parts I am planning to move into the new project are:

* _OpenGL/OSG_ graphics view
* _OpenCL_ bindings
* _SQL_ and _NoSQL_ database interfaces

but I might make also available other pieces as brand new projects, as I did with 
[_QLua_](http://github.com/candycode/qlua) or code snippets as in the case of one of 
my many [_Any type_](http://github.com/candycode/typeless) implementations.

##License

LoCO is distributes under the terms of the 
[_New BSD License_](http://github.com/candycode/loco/blob/master/license/README.md)

##Main features

Use _JavaScript_ to invoke methods and access properties in _QObject_-derived objects.

Connect:

* _JavaScript_ signals to _QObject_ slots
* _QObject_ signals to _JavaScript_ functions and methods
* _QObject_ signals to _QObject_ slots
* Pass _QObject_ pointers to _QObject_ methods from _JavaScript_.

Load _QObject_s from binary plugins.

Distribute applications as a standalone executable with all the resources stored in the executable itself.

Use standard web tools to develp desktop applications.

###GUI

The main _GUI_ toolkit is intended to be _WebKit_, but in order to support native widgets a number of wrappers are already available for accessing system dialogs and controls such as the _MacOS_ drawer and top level menu; others are being added. In the future it will be possible to specify an entire native GUI through _JSON_ and use [Knockoutjs](http://knockoutjs.com/) to manage the user interface
and application logic, as done for web applications.

_HUD_-type interfaces are supported through _WebKit_ or _QGraphicsWidget_s layered on top
of a _QGraphicsView_; a proof of concept was implemented as a plugin, checkout the _gui.js_ and _gui.html_ files [here](http://github.com/candycode/loco/tree/master/modules/plugins/osgview/test).

A minimal _OpenGL_-_WebView_ integration example is available as well:

* [_OpenGL_ application](https://github.com/candycode/loco/blob/master/modules/plugins/opengl-app/GLApp.h):
   handle events received from the run-time
* [_JavaScript_ application](https://github.com/candycode/loco/blob/master/apps/locoplay-scripts/test33-graphics-view-webwindow.js): create GUI then load OpenGL application and connect events coming from the _GUI_ to event handlers in the _OpenGL_ app. 

###WebKit integration

_WebKit_ is exposed to _LoCO_ through a _WebWindow_ or _GraphicsWebWindow_(works inside an
_OpenGL_ context) object.

_WebKit_ events are forwarded to the JavaScript context that creates the _WebKit_ window and can therefore be handled outside the _WebKit JavaScript_ context.

It is possible to inject _JavaScript_ code and add _QObject_-derived types at page loading time.

The page _DOM_ tree is available and can be manipulated from outside the page.

A custom plugin factory is available to add _LoCO QWidget_s directly into a web page. 
[Example here](http://github.com/candycode/loco/tree/master/modules/webplugins/osg-viewer).

_Note_: I plan to keep supporting the _WebKit1_ interface, not _WebKit2_ since it requires one additional process for each web page which is **not** something I want to have in a desktop application. The current version of _QtWebKit_ based on _WebKit_ 2.2 works well and will be 
supported for quite some time anyway with commitments to fix all the high priority bugs, and
commercial support available.

**Sample code:**

Load a webpage and change the _DOM_ tree on the fly setting the background to yellow and
rotating all the ```<div>``` elements.

{% highlight javascript %}

try {

  var print = Loco.console.println;
  var ctx = Loco.ctx;

  // command line
  var cmdParam = ctx.cmdLine()[ctx.cmdLine().length - 1];
  var WEBSITE = cmdParam.lastIndexOf( ".js" ) < 0 ?
                cmdParam : "http://www.nyt.com";

  // create main window
  var ww = Loco.gui.create( "WebWindow" );
  // setup main window
  ww.setAttributes( {DeveloperExtrasEnabled: true,
                     LocalContentCanAccessFileUrls: true,
                     LocalContentCanAccessRemoteUrls: true,
                     AcceleratedCompositingEnabled: true } );
  ww.setEnableContextMenu( true );
  ww.setForwardKeyEvents( true );
  if( !ww.syncLoad( WEBSITE, 5000 ) ) throw "Load failure";
  var elements = ww.findElements( "div" );
  print( elements.length );
  print( elements[ 0 ].attributeNames() );
  print( elements[ 0 ].eval( "this.id" ));
  elements = ww.forEachElement( "*", "this.childNodes.length === 0" );
  print( elements.length );
  elements = ww.forEachElement( "div", "this.style.backgroundColor='yellow'; false;" );
  print( elements.length );
  elements = ww.forEachElement( "div", "this.style['-webkit-transform']='rotate(1deg)'; false;" );
  print( elements.length );
  ww.show();
} catch(e) {
  Loco.console.printerrln(e);
  Loco.ctx.exit( -1 );
}

{% endhighlight %}

###Filters

The code/bytes passed to the _LoCO_ intepreter are transformed trough a chain of filters before the actual code is delivered to the interpreter. This allows to e.g. load a source file and use _Skulpt_ or _CoffeeScript_ to generate _JavaScript_ code on the fly and further pass the generated code to _lint_.

Another use of filters is to distribute applications which read enrypted/obfuscated
source code and decode it on the fly.

**Sample code:**

1. load _CoffeeScript_
2. create a filter named _"coffeescript"_ which invokes the function _loco_coffeeCompile_ defined 
   in the last function parameter; such function takes care of the actual 
   _CoffeeScript_ --> JavaScript translation
3. evaluate code from file telling _LoCO_ that it has to be filtered through the _"coffeescript"_ filter

{% highlight javascript %}

try {
  Loco.console.println("Interpreter: " + Loco.ctx.jsInterpreterName() );
  var WKIT = Loco.ctx.jsInterpreterName().indexOf( "webkit" ) >= 0;
  Loco.ctx.include( "../../filters/coffee-script-1.2.js" );
  if( !WKIT ) {
    alert = Loco.console.println;
  }
  var c = CoffeeScript.compile( "x = 32", {bare: true} );
  Loco.console.println( "COFFEE: x = 32\nJAVASCRIPT:\n" + c );
  Loco.ctx.addScriptFilter( "coffeescript", "loco_coffeeCompile_",
                            "function loco_coffeeCompile_( coffeeCode ) {" +
                            " return CoffeeScript.compile( coffeeCode, {bare: true} );" +
                            "}" );
  Loco.ctx.evalFile( "./test5.coffee", ["coffeescript"] );
  Loco.ctx.exit( 0 );
} catch( e ) {
  Loco.console.printerrln( e );
  Loco.ctx.exit( -1 );
}

{% endhighlight %}

###Synchronous calls

Where applicable I replaced async calls with callbacks with sync calls with soft real-time guarantees i.e. invoke a synchronous function telling it how long you are willing to wait for completion.

E.g.

{% highlight javascript %}

webWindow.syncLoad( "http://www.github.com", 5000 /*ms*/ );

{% endhighlight %}

###QtScript and JavaScriptCore(or V8) support

_JavaScript_ code can be run through either _Qt_'s own script engine or the _JavaScript_ engine embedded in _WebKit_. In both cases the code is _JIT_ compiled before execution, unless
you explicitly disable _JIT_ compilation when building _Qt_.

In case _QtScript_ is used it is possible to remove dependencies on _QtWebKit_ and/or _QtGUI_.

_JavaScript_ code can be evaluated from within _JavaScript_ through the ```eval``` function or(if exposed) ```<LocoContextname>.eval```, in the former case the code is interpreted, in the latter it goes through _JIT_ compilation. It is also possible to entire disable access to run-time code
evaluation in slave contexts if needed.

###Nested contexts

It is possible to create other _JavaScript_ contexts from within any existing _JavaScript_ context and marshal data between parent and child context. This allow the creation of sandboxed contexts with only a subset of the _JavaScript_ environment exposed to scripts.

**Sample code:**

1. create new context and install event handlers for errors or _JavaScript_ console messages
2. enable access to source code passed to the context itself for evaluation
3. add objects to the context, including a reference to the context itself
4. evaluate the code in the new context

{% highlight javascript %}

//check type of current context(WebKit or QtScript)
var WEBKIT = 
  Loco.ctx.jsInterpreterName().indexOf( "webkit" ) >= 0, 
  CONTEXT_TYPE = WEBKIT ? "JavaScriptCoreContext" : "QtScriptContext";
var newCtx = Loco.ctx.create( CONTEXT_TYPE );
newCtx.onError.connect( err );
newCtx.javaScriptConsoleMessage.connect(
 function( msg, line, src ) {
   print( msg + " at line " + line );
 } );
print( "Enable storage of source code passed for evaluation" );
newCtx.storeCode = true;
newCtx.addObject( newCtx, "ctx" );
print( "Added new context reference as 'ctx' into new context itself" );
newCtx.addObject( Loco.console, "io" );
print( "Added 'Loco.console' as 'io' into new context" );
var CODE = "io.println(ctx.code)";
print( "Evaluating code '" + CODE + "' in new context" );
newCtx.eval( CODE ); //prints out code passed to newCtx itself!

{% endhighlight %}

###Custom resource access manager

Network and filesystem access is controlled by resource access managers which can be configured through a regex engine or entirely replaced to:

* specify read/write access permissions for files and directories
* restrict access to specific network resources
* filter and log network requests

**Sample code 1:** enable file and network access from driver(_C++_) application

{% highlight cpp %}
...
loco::App app( qtApp, argc, argv );
#ifdef ACCESS_TO_FILESYSTEM_ENABLED
app.AddModuleToJS( new loco::FileSystem );
app.SetAllowFileAccess( true );
app.SetFilterFileAccess( false );
#endif
#ifdef ACCESS_TO_NETWORK_ENABLED
app.SetAllowNetAccess( true );
app.SetFilterNetRequests( false );
app.AddModuleToJS( new loco::Network );
#endif
{% endhighlight %}

**Sample code 2:** forbid read-write access to files with extension _"config"_

{% highlight javascript %}
...
app.SetAllowFileAccess( true );
app.SetFilterFileAccess( true );
app.SetDenyFileRule( QRegExp( ".*\\.config$" ), QIODevice::ReadWrite );
{% endhighlight %}

Note that access control is not entirely exposed to _JavaScript_, to allow for the creation of binaries that have built-in, user-configurable access control pre-configured inside the interpreter.

This is something I borrowed from the mobile environment, desktop applications are unfortunately still being developed with patterns from the 80s/90s with no concept of security or access control, any desktop application can easily access the file system to read and broadcast data over the internet with little control over it, not to mention direct access to audio/video devices with no access protection at all(this is something I'll be addessing in future releases).

###Custom protocols

Custom protocol handlers can be installed in the web engine to allow for addition of new schemes or filtering of requests for standard schemes.

**Sample code:** create and install protocol handler

{% highlight javascript %}
var customReqHandler = ctx.create( "ProtocolHandler" );
ctx.setEnableCustomProtocolHandlers( true );
ctx.addProtocolHandler( "myprotocol", customReqHandler );
function handleCustomRequest( req, reply ) {
  //generate some text content
  var content = ...
  reply.setHeader( "ContentType", "text/html; charset ASCII" );
  reply.setHeader( "ContentLength", content.length );
  reply.setContent( content );
  reply.setUrl( "file://" );
} 
customReqHandler.handleRequest.connect( handleCustomRequest );
{% endhighlight %}

Note that the custom protocol handler is added into the main context **not** the WebWindow: 
a scheme -> network-request-handler map is stored into the context itself, which allows to match schemes/protocols in every WebWindow with the proper handler.

Overriding the standard "http(s)" scheme it is possible to create proxys or embedded web applications with all the code written in _JavaScript_.

Example:

* [Application](http://github.com/candycode/loco/blob/master/apps/locoplay-scripts/test20-custom-protocol.js)
* [Page](http://github.com/candycode/loco/blob/master/apps/locoplay-scripts/test20-custom-protocol.html)

###Multithreading

Scripts can be run in multiple contexts mapped to different threads.

Two thread objects currently available:

* _Thread_: executes the code once and exits
* _Thread loop_:
  * thread is created and put in a wait state
  * at each request for execution the code is executed and 
    the thread goes then back into the wait state

Input data are passed to threads through their parent context.

Results can be retrieved from the parent context/thread by:

* explicitly joining/sychronizing then reading whatever data shared with the thread
* reading from the thread.data variable which implements future-like behavior and waits until data become available

[A working example](http://github.com/candycode/loco/blob/master/apps/locoplay-scripts/test23-thread-loop.js) is available.

###Network

Support for _tcp_/_udp_ sockets, _http_ and _ssl_ is included.

**Sample code 1:** http request
{% highlight javascript %}
var http = Loco.net.create( "Http" );
var reply = http.get( "http://www.marinij.com", 10000 );
var headers = "";
for( var h in reply.headers ) {
  headers += h + ": ";
  if( reply.headers[ h ] ) headers += reply.headers[ h ];
  headers += "\n";
}
if( headers.length > 0 ) fwrite( "test24-output-headers.txt", headers );
fwrite( "test24-output.html", reply.content );
if( Loco.gui ) {
  var ww = Loco.gui.create( "WebWindow" );
  ww.load( "test24-output.html" );
  ww.show();
}
{% endhighlight %}

**Sample code 2:** ssl socket
{% highlight javascript %} 
var socket = Loco.net.create( "tcp-ssl-socket" );
socket.connectTo( "bugs.kde.org", 443, 5000 );
//if ( !socket.waitForEncrypted( 50000 ) ) throw socket.errorMsg();
socket.write( "GET / HTTP/1.1\r\n"+
              "Host: bugs.kde.org\r\n"+
              "Connection: Close\r\n\r\n" );
var data = "";
while( socket.waitForReadyRead( 5000 ) ) data += socket.readAll();
var suffixLength = 5;
print( data.slice(0,data.length-suffixLength) ); //remove non printable chars
print( "done" );
{% endhighlight %}

A full _SSL_ example is available as well: [encrypted fortune](http://github.com/candycode/loco/blob/master/apps/locoplay-scripts/test28-ssl.js)

It is also possible to use the _WebWindow_ object as a headless web browser to isssue _http_(s) requests, handle responses and save page snapshots to image or PDF files from command line applications.

