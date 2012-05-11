#pragma once
////////////////////////////////////////////////////////////////////////////////
//Copyright (c) 2012, Ugo Varetto
//All rights reserved.
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions are met:
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder nor the
//      names of its contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//DISCLAIMED. IN NO EVENT SHALL UGO VARETTO BE LIABLE FOR ANY
//DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////


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

// example of unrelated plugin with no dependencies on LoCO

// dummy interface required to derive from and to declare through Q_INTERFACES
struct IDummy {};
Q_DECLARE_INTERFACE(IDummy,"dummy")

class OSGViewerPlugin : public QWidget, public IDummy  {
    Q_OBJECT
    Q_PROPERTY( QString name READ name )
    Q_PROPERTY( QString version READ version )
    Q_PROPERTY( QString author READ author )
    Q_INTERFACES( IDummy )
public:
    OSGViewerPlugin( QWidget* parent = 0 ) : QWidget( parent ),
        name_( "OSG Viewer web plugin " ),
        version_( "1.0" ),
        author_( "UV" ) {
    	///@todo add additional models
    	viewer_.setThreadingModel( osgViewer::CompositeViewer::SingleThreaded );
        QGridLayout* l = new QGridLayout;
        l->addWidget( AddViewWidget( CreateCamera( 0, 0, 100, 100  ) ), 0, 0 );
        setLayout( l );
        QWidget::connect( &timer_, SIGNAL( timeout() ), this, SLOT( update() ) );
        timer_.start( 20 );

    }

    const QString& name() const { return name_; }
    const QString& version() const { return version_; }
    const QString& author() const { return author_; }

protected:
    void paintEvent( QPaintEvent* pe ) {
    	viewer_.frame();

    }

signals:
    void onError( const QString& );

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
           view->requestContinuousUpdate( false );
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
    QTimer timer_;
    QString name_, version_, author_;
};

Q_EXPORT_PLUGIN2( osgviewer, OSGViewerPlugin )
