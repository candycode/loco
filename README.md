## LoCO

LoCO is a set of C++ classes that make it easy to create command-line and GUI
applications with any language that compile to Javascript.
Javascript is used to glue together binary components, optionally loaded
at run-time, developed in C++.
Objects are connected through signals/slots or by direct reference through
a QObject pointer, giving to the objects the reponsibility to check the
interface semantic or the object type.
Note that with Qt it is possible to perform duck-typing with C++: only
check if a property or method is available, **not** if an object is of
a specific type. 

This project started several years ago when I got tired of spending time
writing C/C++ code to build MVP/MVVM application logic like and bind UI
events to callbacks with MOTIF/MFC/GTK/Qt/WPF...
After some time spent looking into various scripting languages and their
bindings to GUI frameworks I settled on Qt simply because it's been and
still is the fastest path to building cross-platform applications
scriptable in a widespread scripting language such as ECMAScript/Javascript. 
The current the code is a stripped down, cleaned-up, partially rewritten
version of a larger and much garbled project which also had some Lua, 
Python and Scheme bindings; the only part I'm planning to move into the
new project is the GL/OSG graphics view stuff, but I might make available
other pieces as brand new project, as I did with QLua, available [here](/candycode/qlua).

 
Another goal I've had for quite some time is to build desktop applications
with standard Javascript libraries such as Knockout, jQuery(UI) and others.
LoCO has bindings for WebKit which is intended to be the main toolkit for
developing GUI applications, including HUD type of interfaces on top of
OpenGL/OSG.

A minimal number of native UI widgets is support to access system
dialogs and some non-standard(across operating system) controls such as
the MacOS drawer and top menu bar.
