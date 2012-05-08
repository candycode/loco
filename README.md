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

A minimal number of native UI widgets is supported mainly to access system
dialogs and some non-standard(across operating system) controls such as
the MacOS drawer and top menu bar.

The code/bytes passed to the LoCO intepreter are transformed trough
a chain of filters before the actual code is delivered to the interpreter.
This allows to e.g. load a source file and use Skulpt or CoffeeScript
to generate JavaScript code on the fly and further pass the generated
code to lint.

## Main features

Use JavaScript to invoke methods and access properties in QObject-derived
objects.

Connect:
* JavaScript signals to QObject slots
* QObject signals to JavaScript functions
* QObject signals to QObject slots 

Pass QObject pointers to QObject methods.

Load QObjects from binary plugins.







## History and status
This project started several years ago when I got tired of spending time
writing C/C++ code to build MVP/MVVM application logic like and bind UI
events to callbacks with MOTIF/MFC/GTK/Qt/WPF...
After some time spent looking into various scripting languages and their
bindings to GUI frameworks I settled on Qt simply because it's been and
still is the fastest path to building cross-platform applications
scriptable in a widespread scripting language such as ECMAScript/JavaScript. 
The current the code is a stripped down, cleaned-up, partially rewritten
version of a larger and much garbled project which also had some Lua, 
Python and Scheme bindings; the only part I'm planning to move into the
new project is the GL/OSG graphics view and probably some OpenCL stuff;
but I might make also available other pieces as brand new project,
as I did with QLua, available [here](/candycode/qlua).

