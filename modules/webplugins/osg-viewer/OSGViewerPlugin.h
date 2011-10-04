#pragma once
//#SRCHEADER

#include <string>
#include <stdexcept>

#include <osg/Camera>
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile>
#include <osgQt/GraphicsWindowQt>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>

#include <QObject>
#include <QtPlugin>
#include <QWidget>
#include <QString>
#include <QGridLayout>
#include <QtCore/QTimer>
#include <QStringList>


struct IDummy : QWidget {
    Q_OBJECT
public:
    IDummy( QWidget* parent ) : QWidget( parent ) {}
signals:
    void onError( const QString& );
};

Q_DECLARE_INTERFACE(IDummy,"dummy")


class OSGViewerPlugin : public IDummy  {
    Q_OBJECT
    Q_INTERFACES( IDummy )
public:
    OSGViewerPlugin( QWidget* parent = 0 ) : IDummy( parent ) {
    	///@todo add additional models
    	viewer_.setThreadingModel( osgViewer::CompositeViewer::SingleThreaded );
    	//1) setup layout
        QGridLayout* l = new QGridLayout;
        l->addWidget( AddViewWidget( CreateCamera( 0, 0, 100, 100  ) ), 0, 0 );
        setLayout( l );
        QWidget::connect( &timer_, SIGNAL( timeout() ), this, SLOT( update() ) );
        timer_.start( 20 );
        emit onError( QString( "HEY" ) );
    }
protected:
    void paintEvent( QPaintEvent* pe ) {
    	viewer_.frame();
    }
public slots:
    void loadScene( const QString& path ) {
       	osg::Node* scene = osgDB::readNodeFile( path.toStdString() );
    	if( !scene ) {
    		emit onError( "Cannot load " + path );
    		return;
    	}
    	viewer_.getView( 0 )->setSceneData( scene );
    }
    void init( const QStringList& argNames, const QStringList& argValues ) {
       for( int i = 0; i != argNames.size(); ++i ) {
           if( argNames[ i ] == "bgcolor" ) {
        		if( argValues.size() <= i ) {
                    throw std::runtime_error( "Number of arguments different from number of values" );
        	        return;
        		}
        	    QStringList rgba = argValues[ i ].split( "," );
        		if( rgba.length() != 4 ) {
        			throw std::runtime_error( "Wrong background color format" );
        			return;
        		}
        		viewer_.getView( 0 )->getCamera()->setClearColor( osg::Vec4(
        				                                          rgba[ 0 ].toFloat(),
        				                                          rgba[ 1 ].toFloat(),
        				                                          rgba[ 2 ].toFloat(),
        				                                          rgba[ 3 ].toFloat() ) );
        	}
        }
    }
private:
    QWidget* AddViewWidget( osg::Camera* camera ) {

           osgViewer::View* view = new osgViewer::View;
           view->setCamera( camera );
           viewer_.addView( view );

           view->addEventHandler( new osgViewer::StatsHandler );
           view->setCameraManipulator( new osgGA::TrackballManipulator );

           osgQt::GraphicsWindowQt* gw = dynamic_cast<osgQt::GraphicsWindowQt*>( camera->getGraphicsContext() );
           return gw ? gw->getGLWidget() : NULL;
     }
     osg::Camera* CreateCamera( int x, int y, int w, int h,
    		                    const std::string& name="", bool windowDecoration=false ) {
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

           osg::ref_ptr<osg::Camera> camera = new osg::Camera;
           camera->setGraphicsContext( new osgQt::GraphicsWindowQt(traits.get()) );

           camera->setClearColor( osg::Vec4(0.2, 0.2, 0.6, 1.0) );
           camera->setViewport( new osg::Viewport(0, 0, traits->width, traits->height) );
           camera->setProjectionMatrixAsPerspective(
               30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0f, 10000.0f );
           return camera.release();
       }
private:
    osgViewer::CompositeViewer viewer_;
    //osg::ref_ptr< osgQt::GLWidget> osgWidget_;
    osg::ref_ptr< osg::Node > scene_;
    QTimer timer_;
};

Q_EXPORT_PLUGIN2( osgviewer, OSGViewerPlugin )
