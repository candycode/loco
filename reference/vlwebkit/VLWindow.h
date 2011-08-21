#pragma once

#include <QtGui/QWidget>
#include <vlCore/VisualizationLibrary.hpp>
#include <vlQt4/Qt4Widget.hpp>
#include <QtGui/QVBoxLayout>
#include "Applets/App_RotatingCube.hpp"
#include <iostream>

//------------------------------------------------------------------------------
class VLWindow : public QWidget {
	Q_OBJECT
public:
	VLWindow( QWidget* parent = 0 ) : QWidget( parent ) {
		/* init Visualization Library */
		vl::VisualizationLibrary::init();

		/* setup the OpenGL context format */
		vl::OpenGLContextFormat format;
		format.setDoubleBuffer(true);
		format.setRGBABits( 8,8,8,8 );
		format.setDepthBufferBits(24);
		format.setStencilBufferBits(8);
		format.setFullscreen(false);
		//format.setMultisampleSamples(16);
		//format.setMultisample(true);

		/* create the applet to be run */
        applet = new App_RotatingCube;
		applet->initialize();
		/* create a native Qt4 window */
		qt4_window = new vlQt4::Qt4Widget;


		QVBoxLayout *layout = new QVBoxLayout;
		layout->addWidget(qt4_window.get());
   
        this->setLayout(layout);

		/* bind the applet so it receives all the GUI events related to the OpenGLContext */
		qt4_window->addEventListener(applet.get());
		/* target the window so we can render on it */
		applet->rendering()->as<vl::Rendering>()->renderer()->setRenderTarget( qt4_window->renderTarget() );
		/* black background */
		applet->rendering()->as<vl::Rendering>()->camera()->viewport()->setClearColor( vl::black );
		/* define the camera position and orientation */
		vl::vec3 eye    = vl::vec3(0,10,35); // camera position
		vl::vec3 center = vl::vec3(0,0,0);   // point the camera is looking at
		vl::vec3 up     = vl::vec3(0,1,0);   // up direction
		vl::mat4 view_mat = vl::mat4::getLookAt(eye, center, up).getInverse();
		applet->rendering()->as<vl::Rendering>()->camera()->setViewMatrix( view_mat );
		/* Initialize the OpenGL context and window properties */
		int x = 10;
		int y = 10;
		int width = 512;
		int height= 512;
		qt4_window->initQt4Widget( "Visualization Library on Qt4 - Rotating Cube", format, NULL, x, y, width, height );
	}
	~VLWindow() {
		/* shutdown Visualization Library */
		vl::VisualizationLibrary::shutdown();
	}
	public slots:
		void translate( float x, float y, float z ) {
			vl::Transform& t = applet->getTransform();
			t.translate( x, y, z );
			refresh();
		}
		void resize( int w, int h ) { QWidget::resize(w,h); }
		void refresh() { qt4_window->paintGL(); }
private:
	vl::ref<App_RotatingCube>  applet;
	vl::ref<vlQt4::Qt4Widget> qt4_window;
};