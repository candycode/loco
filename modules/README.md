Run-time loadable objects in the form of QtPlugins or QtWebPlugins.

*`plugins`: run-time loadable objects
*`webplugins`: run-time QWidget-derived object to be embedded into html pages

Every plugin comes with JavaScript driver program to test its functionality.

These plugins are just examples of how to create run-time loadable modules 
accessible from within _LoCO_ and do have little or in most cases no dependency on _LoCO_ itself.
Looking from a different perspecive they also show how to use _LoCO_ to load,
inspect and use any pre-built Qt plugin. 
