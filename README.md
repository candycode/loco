## LoCO - Loosely Coupled Objects

_LoCO_ is a set of C++ classes that make it easy to create command-line and GUI
applications with any language that compiles to JavaScript.
JavaScript is used to glue together binary components, optionally loaded
at run-time, developed in C++.
Objects are connected through signals/slots or by direct reference through
a QObject pointer, giving to the objects the reponsibility to check the
interface semantic or the object type.
Note that with Qt it is possible to perform duck-typing with C++: only
check if a property or method is available, **not** if an object is of
a specific type. 

A goal I've had for quite some time is to build desktop applications
with standard JavaScript libraries such as Knockout, jQuery(UI) and others;
_LoCO_ makes this possible through bindings for WebKit which is intended to be the main toolkit for
developing GUI applications, including HUD type of interfaces on top of
OpenGL/OpenSceneGraph.

A minimal number of native UI widgets is supported mainly to access system
dialogs and some non-standard(across operating systems) controls such as
the MacOS drawer and top menu bar.

Have a look at the provided _[locoplay](/candycode/loco/tree/master/apps/locoplay)_ 
application to get a feeling of what it
takes to create a basic _LoCO_-based application which executes scripts
within a custom taylored JavaScript environment. Also have a look at
the cmake configuration file to learn how to bundle all the scripts and
resources in a single file.

[Scripts that work with locoplay](/candycode/loco/tree/master/apps/locoplay-scripts)


Note that there is no plan to support any mobile platform at this time
since after experimenting with different solutions, and having worked
for a few years in mobile development I believe there is no need
of such a toolkit for mobile platforms thanks to their (now)polished and
feature rich APIs and the availability of platforms such as PhoneGap,
Appcelerator, Xamarin and others.

FYI the use cases I'm looking at are:
* quickly develop small GUI applications and command line tools with CoffeeScript/JavaScript
* visualization/analysis applications for scientific data
* develop CAD/3D content creation apps
* experiments with image processing/computer vision
* GUIs for tweaking/configuring/steering experimental scientific applications developed in CUDA, OpenCL and MPI
* interface to hardware devices(e.g. Arduino) and access to audio/video input/output ports

##History and status

This project started several years ago when I got tired of spending time
writing C/C++ code to build MVP/MVVM/MVC application logic and binding UI
events to callbacks with MOTIF/MFC/GTK/Qt/WPF... + a few mobile frameworks.
After some time spent experimenting with different scripting languages and their
bindings to GUI frameworks I settled on Qt for desktop applications simply because 
it's been and still is the fastest path to building cross-platform applications
scriptable in a widespread scripting language such as ECMAScript/JavaScript. 

###Status

The current code is a stripped down, cleaned-up, partially rewritten
version of a larger and much garbled project which also had some Lua, 
Python, Tcl and Scheme bindings; the only additional parts I'm planning to move into the
new project are:

 * OpenGL/OSG graphics view
 * OpenCL bindings
 * SQL and NoSQL database interfaces
 
but I might make also available other pieces as brand new projects,
as I did with [QLua](/candycode/qlua) or code snippets as in the case of one
of my many _[Any type](/candycode/typeless)_ implementation.

_libloco_ will maintain its current interface, only additions,
additional documentation and fixes are planned.

_locoplay_ should also stay as it is.

Everything else, namely locoplay scripts and modules might be removed/updated/added/changed.

Design documents and UML diagrams created with the excellent [yul](http://yuml.me/) tool might
point pop up in the public repository at some point.

###License

_LoCO_ is distributes under the terms of the [New BSD License](/candycode/loco/blob/master/license/README.md)


## Main features

Use JavaScript to invoke methods and access properties in QObject-derived
objects.

Connect:
* JavaScript signals to QObject slots
* QObject signals to JavaScript functions
* QObject signals to QObject slots 

Pass QObject pointers to QObject methods from JavaScript.

Load QObjects from binary plugins.

Distribute applications as a standalone executable with all the resources
stored in the executable itself.

Use standard Web tools to develp desktop applications.

### GUI

The main GUI toolkit is intended to be WebKit but in order to support
native widgets a  number of wrappers are already available for 
accessing system dialogs and controls such as the MacOS drawer and top
level menu; more are being added. In the future it will be possible
to specify an entire native GUI through JSON and use [Knockoutjs](http://knockoutjs.com)
to manage the user interface as done for web applications.

HUD-type interfaces are going to be supported through WebKit or 
QGraphicsWidgets layered on top of a QGraphicsView; a proof
of concept was implemented as a plugin, checkout the _gui.js_
and _gui.html_ files [here](/candycode/loco/tree/master/modules/plugins/osgview/test). 


### WebKit integration

WebKit is exposed to _LoCO_ through a WebWindow object.

WebKit events are forwarded to the JavaScript context that creates
the WebKit window and can therefore be handled outside the WebKit
JavaScript context.

It is possible to _inject_ JavaScript code and add QObject-derived types
at page loading time.

The page _DOM_ tree is available and can be manipulated from outside
the page.

A custom plugin factory is available to add _LoCO_ QWidgets directly into
a web page. [Example here](/candycode/loco/tree/master/modules/webplugins/osg-viewer).


_Note_: I plan to keep supporting the WebKit1 interface, not WebKit2
since it requires one additional process for each web page which is 
__not__ something I want to have in a Desktop application. The current
version of QtWebKit based on WebKit 2.2 works well and will be supported
for quite some time anyway with commitments to fix all the high priority bugs.

**Sample code**:

Load a webpage and change the _DOM_ tree on the fly setting the background to yellow
and rotating all the ```<div>``` elements.

```javascript
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
```

### Filters

The code/bytes passed to the _LoCO_ intepreter are transformed trough
a chain of filters before the actual code is delivered to the interpreter.
This allows to e.g. load a source file and use Skulpt or CoffeeScript
to generate JavaScript code on the fly and further pass the generated
code to lint.

**Sample code**: 

1. load coffeescript 
2. create a filter named _coffeescript_ which invokes the function *loco_coffeeCompile* defined in the last function parameter; such function takes care of the actual CoffeeScript -> JavaScript translation
3. evaluate code from file telling _LoCO_ that it has to be filtered with the _coffeescript_ filter   

```javascript
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
```

### Synchronous calls

Where applicable I replaced async calls with callbacks with sync calls with
soft real-time guarantees i.e. invoke a synchronous function telling it how
long you are willing to wait for completion.

E.g.

```javascript
webWindow.syncLoad( "http://www.github.com", 5000 /*ms*/ );
```

### QtScript and JavaScriptCore(or V8) support

JavaScript code can be run through either Qt's own script
engine or the JavaScript engine embedded in WebKit.
In both cases the code is JIT compiled before execution.

In case QtScript is used it is possible to remove dependencies on QtWebKit
and/or QtGUI.

JavaScript code can be evaluated from within JavaScript through
the ```eval``` function or(if exposed) ```Context.eval```, in the
former case the code is interpreted, in the latter it goes through JIT
compilation.


### Nested contexts

It is possible to create other JavaScript contexts from within
any existing JavaScript context and marshal data between parent
and child context. This allow the creation of sandboxed contexts
with only a subset of the JavaScript environment exposed to scripts.

**Sample code**:

1. create new context and install event handlers for errors or
   javascript console messages
2. enable access to source code passed to the context itself
   for evaluation
3. add objects to the context, including a reference to the context itself 
4. evaluate the code in the new context

```javascript
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
```

### Custom resource access manager

Network and filesystem access is controlled by resource access managers
which can be configured through a regex engine or entirely replaced to:

* specify read/write access permissions for files and directories
* restrict access to specific network resources
* filter and log network requests

**Sample code 1**: enable file and network access from driver(C++) application

```c++
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

```

**Sample code 2**: forbid read-write access to files with extension "_config_"

```cpp
...
app.SetAllowFileAccess( true );
app.SetFilterFileAccess( true );
app.SetDenyFileRule( QRegExp( ".*\\.config$" ), QIODevice::ReadWrite );
```

Note that access control is not entirely exposed to JavaScript, to allow
for the creation of binaries that have built-in, user-configurable access
control at the interpreter level.

This is something I borrowed from the mobile environment, Desktop applications
are unfortunately still being developed with patterns from the 80s/90s with
no concept of security or access control, any desktop application can easily
access the file system to read and broadcast data over the internet with little control
over it, not to mention direct access to audio/video devices with no access protection
at all(this is something I'll be addessing in future releases).


### Custom protocols

Custom protocol handlers can be installed in the web engine to allow
for addition of new schemes or filtering of requests for standard schemes.

**Sample code**: create and install protocol handler

```javascript
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
```

Note that the custom protocol handler is added into the context **not**
the WebWindow: a scheme -> network-request-handler map is stored
into the context itself, which allows to match schemes/protocols in
every WebWindow with the proper handler.

Overriding the standard _"http(s)"_ scheme it is possible to create
proxys or embedded web applications with all the code written in
JavaScript.    

Example:

[Application](/candycode/loco/blob/master/apps/locoplay-scripts/test20-custom-protocol.js)

[Page](/candycode/loco/blob/master/apps/locoplay-scripts/test20-custom-protocol.html)
 

### Multithreading

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
* reading from the _thread.data_ variable which implements future-like behavior and waits
  until data become available

[A working example example](/candycode/loco/blob/master/apps/locoplay-scripts/test23-thread-loop.js)
is available.

### Network

Support for tcp/udp sockets, http and ssl is included.

Sample code 1: http request

```javascript
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
```

Sample code 2: ssl socket

```javascript
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
```

A full SSL example is available as well:
 [encrypted fortune](/candycode/loco/blob/master/apps/locoplay-scripts/test28-ssl.js)

It is also possible to use the WebWindow object as a headless web browser to
isssue http(s) requests, handle responses and save page snapshots to image or PDF
files from command line applications.

