## LoCO - Loosely Coupled Objects

_LoCO_ is a set of C++ classes that make it easy to create command-line and GUI
applications with any language that compiles to JavaScript.
JavaScript is used to glue together binary components, optionally loaded
at run-time, developed in C++.
Objects are connected through signals/slots or by direct reference through
a QObject pointer, giving to the objects the responsibility to check the
interface semantics or the object type.
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
[Scripts that work with locoplay](https://github.com/candycode/loco/tree/master/apps/locoplay-scripts)

Note that there is no plan to support any mobile platform at this time
since after experimenting with different solutions, and having worked
for a few years in mobile development I believe there is no need
of such a toolkit for mobile platforms thanks to their (now) polished and
feature rich APIs and the availability of platforms such as PhoneGap,
Appcelerator, Xamarin and others.

FYI the use cases I'm looking at are:

* quickly develop small GUI applications and command line tools with CoffeeScript/JavaScript
* visualization/analysis applications for scientific data
* develop CAD/3D content creation apps
* experiments with image processing/computer vision
* GUIs for tweaking/configuring/steering experimental scientific applications developed in CUDA, OpenCL and MPI
* interface to hardware devices(e.g. Arduino) and access to audio/video input/output ports

The end goal is in general to create working prototypes of any kind as fast as possible through mashups
of pre-existing modules.  

Additional information is available on the official [LoCO website](http://locojs.net).

To get an idea of the requirements I looked at when I designed LoCO have a look at the 
[Specification](https://github.com/candycode/loco/wiki/Specification) page.

Developer's guide is in the making. Some design documents and accompanying UML diagrams created 
with the great [yUML](http://yuml.me/) tool will be made available as well.

This GitHub project is used to support research, design and development and it is subject to frequent changes, also
acting as a scratchpad.

Once things such as documentation and tutorials become stable they are moved to the official user-facing
[LoCO](http://locojs.net) website.




