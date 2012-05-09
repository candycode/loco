## LoCO - Loosely Coupled Objects

LoCO is a set of C++ classes that make it easy to create command-line and GUI
applications with any language that compile to JavaScript.
JavaScript is used to glue together binary components, optionally loaded
at run-time, developed in C++.
Objects are connected through signals/slots or by direct reference through
a QObject pointer, giving to the objects the reponsibility to check the
interface semantic or the object type.
Note that with Qt it is possible to perform duck-typing with C++: only
check if a property or method is available, **not** if an object is of
a specific type. 

A goal I've had for quite some time is to build desktop applications
with standard JavaScript libraries such as Knockout, jQuery(UI) and others.
LoCO has bindings for WebKit which is intended to be the main toolkit for
developing GUI applications, including HUD type of interfaces on top of
OpenGL/OpenSceneGraph.

Here is come code that opens a webpage and changes the DOM on the fly
at load time

A minimal number of native UI widgets is supported mainly to access system
dialogs and some non-standard(across operating system) controls such as
the MacOS drawer and top menu bar.

Have a look at the provided _[locoplay](https://github.com/candycode/loco/blob/master/apps/locoplay/locoplay.cpp)_ 
app to get a feeling of what it
takes to create a basic LoCO-based application which executes scripts
within a custom taylored JavaScript environment. Also have a look at
the cmake configuration file to learn how to bundle all the scripts and
resources in a single file.

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

## Main features

Use JavaScript to invoke methods and access properties in QObject-derived
objects.

Connect:
* JavaScript signals to QObject slots
* QObject signals to JavaScript functions
* QObject signals to QObject slots 

Pass QObject pointers to QObject methods.

Load QObjects from binary plugins.

Distribute applications as a standalone executable with all the resources
stored in the executable itself.

### GUI

The main GUI toolkit is intended to be WebKit but in order to support
native widgets a  number of wrappers are already available for 
accessing system dialogs and controls such as the MacOS drawer and top
level menu; more are being added. In the future it will be possible
to specify an entire native GUI through JSON.

HUD-type interfaces are going to be supported through WebKit or 
QGraphicsWidgets layered on top of a QGraphicsView; a proof
of concept was implemented as a plugin, checkout the _gui.js_
and _gui.html_ files [here](https://github.com/candycode/loco/tree/master/modules/plugins/osgview/test). 


### WebKit integration

WebKit is exposed to LoCO through a WebWindow object.

WebKit events are forwarded to the JavaScript context that creates
the WebKit window and can therefore be handled outside the WebKit
JavaScript context.

It is possible to _inject_ JavaScript code and add QObject-derived type
at page loading time.

The page _DOM_ tree is available and can be manipulated from outside
the page.

A custom plugin factory is available to add LoCO QWidgets directly into
a web page. Example [here](https://github.com/candycode/loco/tree/master/modules/webplugins/osg-viewer).


_Note_: I plan to keep supporting the WebKit1 interface, not WebKit2
since it requires two processes to for each open WebView which is 
__not__ something I want to have in a Desktop application. The current
version of QtWebKit based on WebKit 2.2 works well and will be supported
for quite some time anyway with commitment to fix all the high priority bugs.

Sample code:

Load a webpage and change the _DOM_ tree on the fly setting the background to yellow
and rotating the ```<div>``` elements.

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

The code/bytes passed to the LoCO intepreter are transformed trough
a chain of filters before the actual code is delivered to the interpreter.
This allows to e.g. load a source file and use Skulpt or CoffeeScript
to generate JavaScript code on the fly and further pass the generated
code to lint.

Sample code: 

1. load coffeescript 
2. create a filter named _coffeescript_ which invokes invokes the function *loco_coffeeCompile* defined in the last function parameter
3. evaluate code from file telling LoCO that it has to be filtered with the _coffeescript_ filter   

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
soft real-time guarantees.

E.g. 
```webWindow.syncLoad( "http://www.autodesk.com", 5000 /*ms*/ );```

### QtScript and JavaScriptCore(or V8) support

JavaScript code can be passed run through either Qt's own script
engine or the JavaScript engine embedded in WebKit. In case
QtScript is used it is possible to remove dependencies on QtWebKit
and/or QtGUI.


### Nested contexts

It is possible to create other JavaScript contexts from within
any existing JavaScript context and marshal data between parent
and child context. This allow the creation of sandboxed contexts
with only a subset of the JavaScript environment exposed to scripts.

<CODE SAMPLE>

### Custom resource access manager

Network and filesystem access is controlled by resource access managers
which can be configured through a regex engine or entirely replaced to:

* specify read/write access to files and directories
* restrict access to specific network resources
* filter and log network requests


<CODE SAMPLE>

### Custom protocols

Custom protocol handlers can be installed in the web engine to allow
for addition of new schemes or filtering of requests for standard schemes. 

### Multithreading

Scripts can be run in multiple contexts mapped to different threads.

<CODE SAMPLE>

### Network

Support for tcp/udp sockets and ssl is included.
It is possible to use the WebWindow object as a headless web browser to
isssue http(s) requests and handle responses from command line applications.

<CODE SAMPLE>

## History and status

This project started several years ago when I got tired of spending time
writing C/C++ code to build MVP/MVVM/MVC application logic and binding UI
events to callbacks with MOTIF/MFC/GTK/Qt/WPF... + a few mobile frameworks.
After some time spent experimenting with different scripting languages and their
bindings to GUI frameworks I settled on Qt for desktop applications simply because 
it's been and still is the fastest path to building cross-platform applications
scriptable in a widespread scripting language such as ECMAScript/JavaScript. 
The current code is a stripped down, cleaned-up, partially rewritten
version of a larger and much garbled project which also had some Lua, 
Python and Scheme bindings; the only part I'm planning to move into the
new project is the GL/OSG graphics view and probably some OpenCL stuff;
but I might make also available other pieces as brand new project,
as I did with [QLua](/candycode/qlua).

