## LoCO

This project started several years ago when I got tired of spending time writing C/C++ code to build MVP/MVVM application logic like and bind UI events to callbacks with MOTIF/MFC/GTK/Qt/WPF...
After some time spent looking into various scripting languages and their bindings to GUI frameworks I settled on Qt simply because it's been and still is the fastest path to build cross-platform applications scriptable in a widespread scripting language such as ECMAScript/Javascript.  

LoCO is a set of C++ classes that make it easy to create command-line and GUI applications with any language that compile to Javascript, it also makes it easy to load objects from dynamic libraries at run-time.

Another goal I've had for quite some time is to build desktop applications with standard Javascript libraries such as Knockout, jQuery(UI) and others. For this reason I did spend some time exploring the use of QtWebKit as the main UI toolkit. LoCO does include classes for creating QtWebKit based UIs and a minimal set of wrapper classes for accessing system windows and native controls such as MacOS top menu and drawer. 
