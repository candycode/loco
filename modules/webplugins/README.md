Web plugins to be embedded into web pages.
These plugins have basically no dependency on _LoCO_ and show how to use generic web plugins
from within the _LoCO_ framework.

* `gl`: widget directly derived from QGLWidget embedding an osg::Viewer
* `osg-viewer`: QWidget embedding the GLWidget returned by `osgQt::GraphicsWindowQt::getGLWidget`
