#pragma once
//#SRCHEADER

#include <string>

#include <osg/Camera>
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile>
#include <osgQt/GraphicsWindowQt>
#include <osgViewer/Viewer>

#include <QObject>
#include <QtPlugin>
#include <QWidget>
#include <QString>
#include <QHBoxLayout>
#include <QtCore/QTimer>



class OSGViewerPlugin : public QWidget  {
    Q_OBJECT
    Q_INTERFACES( QWidget )
public:
    OSGViewerPlugin( QWidget* parent = 0 ) : QWidget( parent ),
        viewer_( new osgViewer::Viewer ) {
    	//1) setup layout
        SetupCamera( viewer_->getCamera(), 0, 0, 400, 400 );
        osgQt::GraphicsWindowQt* gw =
        		dynamic_cast< osgQt::GraphicsWindowQt* >( viewer_->getCamera()->getGraphicsContext() );
      	if( !gw ) {
      		emit onError( "Error creating graphics context" );
      		return;
      	}
        QHBoxLayout* l = new QHBoxLayout;
        l->addWidget( gw->getGLWidget() );
        connect( &timer_, SIGNAL( timeout() ), this, SLOT( update() ) );
    }
protected:
    void paintEvent( QPaintEvent* pe ) {
    	QWidget::paintEvent( pe );
    	viewer_->frame();
    }
signals:
    void onError( const QString& msg );
public slots:
    void loadScene( const QString& path ) {
    	osg::Node* scene = osgDB::readNodeFile( path.toStdString() );
    	if( !scene ) {
    		emit onError( "Cannot load " + path );
    		return;
    	}
    	scene_ = scene;
    	viewer_->setSceneData( osg::get_pointer( scene_ ) );
    }
private:
    void SetupCamera( osg::Camera* camera, int x, int y, int w, int h, const std::string& name="",
    		          bool windowDecoration = false ) {
           osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
           osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
           traits->windowName = name;
           traits->windowDecoration = windowDecoration;
           traits->x = x;
           traits->y = y;
           traits->width = w;
           traits->height = h;
           traits->doubleBuffer = true;
           traits->alpha = ds->getMinimumNumAlphaBits();
           traits->stencil = ds->getMinimumNumStencilBits();
           traits->sampleBuffers = ds->getMultiSamples();
           traits->samples = ds->getNumMultiSamples();
           camera->setGraphicsContext( new osgQt::GraphicsWindowQt(traits.get()) );
           camera->setClearColor( osg::Vec4(0.2, 0.2, 0.6, 1.0) );
           camera->setViewport( new osg::Viewport(0, 0, traits->width, traits->height) );
           camera->setProjectionMatrixAsPerspective(
               30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0f, 10000.0f );
       }
private:
    osg::ref_ptr< osgViewer::Viewer > viewer_;
    //osg::ref_ptr< osgQt::GLWidget> osgWidget_;
    osg::ref_ptr< osg::Node > scene_;
    QTimer timer_;
};

Q_EXPORT_PLUGIN2( osgviewer, OSGViewerPlugin )
