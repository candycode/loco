Run-time loadable objects in the form of QtPlugins or QtWebPlugins.

* `plugins`: run-time loadable objects
* `webplugins`: run-time QWidget-derived object to be embedded into html pages

Most plugins comes with a JavaScript driver program to test its functionality. The
ones that do not have a driver program are intented to be loaded from scripts
in the `locoplay-scripts` directory.

These plugins are just examples of how to create run-time loadable modules 
accessible from within _LoCO_ and do have little or in most cases no dependency on _LoCO_ itself.
Looking from a different perspecive they also show how to use _LoCO_ to load,
inspect and use any pre-built Qt plugin. 

-> ![rtaudio](/candycode/loco/raw/master/modules/plugins/rtaudio/test/spectrum-analyzer/sshot.png) <-

![rtaudio](https://github.com/candycode/loco/raw/master/modules/plugins/osgview/test/sshot.png)