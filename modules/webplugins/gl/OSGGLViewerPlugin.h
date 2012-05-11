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
#include <osgViewer/GraphicsWindow>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <QResizeEvent>
#include <QObject>
#include <QtPlugin>
#include <QWidget>
#include <QString>
#include <QGridLayout>
#include <QtCore/QTimer>
#include <QStringList>
#include <QtOpenGL/QGLWidget>

// example of unrelated plugin with no dependencies on LoCO

// dummy interface required to derive from and to declare through Q_INTERFACES
struct IDummy {};
Q_DECLARE_INTERFACE(IDummy,"dummy")

class OSGViewerPlugin : public QGLWidget, public IDummy  {
    Q_OBJECT
    Q_PROPERTY( QString name READ name )
    Q_PROPERTY( QString version READ version )
    Q_PROPERTY( QString author READ author )
    Q_INTERFACES( IDummy )
public:
    OSGViewerPlugin( QWidget* parent = 0 ) : QGLWidget( parent ),
        name_( "OSG Viewer web plugin " ),
        version_( "1.0" ),
        author_( "UV" ) {

        gw_ = new osgViewer::GraphicsWindowEmbedded( 0, 0, width(), height() );
        setFocusPolicy( Qt::ClickFocus );
        viewer_.getCamera()->setViewport( new osg::Viewport( 0, 0, 200, 200 ) );
        viewer_.getCamera()->setProjectionMatrixAsPerspective(
                30.0f, static_cast< double >( 200 )/ static_cast< double >( 200 ),
                                              1.0f, 10000.0f);
        viewer_.getCamera()->setGraphicsContext( osg::get_pointer( gw_ ) );
        viewer_.setThreadingModel( osgViewer::Viewer::SingleThreaded );
        viewer_.addEventHandler( new osgViewer::StatsHandler );
        viewer_.setCameraManipulator( new osgGA::TrackballManipulator );
        timer_.setInterval( 20 );
        connect( &timer_, SIGNAL( timeout() ), this, SLOT( update() ) );
        timer_.start();

    }

    const QString& name() const { return name_; }
    const QString& version() const { return version_; }
    const QString& author() const { return author_; }

protected:
    void resizeEvent( QResizeEvent* event ) {
        viewer_.getCamera()->setViewport(new osg::Viewport(0,0,width(),height()));
        viewer_.getCamera()->setProjectionMatrixAsPerspective(30.0f,
                static_cast<double>(event->size().width())/static_cast<double>(event->size().height()),
                1.0f, 10000.0f);

    }

    void paintGL() {
        viewer_.frame();
    }

    void keyPressEvent( QKeyEvent* event ) {
        gw_->getEventQueue()->keyPress( (osgGA::GUIEventAdapter::KeySymbol) *(event->text().toAscii().data() ) );

    }

    void keyReleaseEvent( QKeyEvent* event ) {
        gw_->getEventQueue()->keyRelease( (osgGA::GUIEventAdapter::KeySymbol) *(event->text().toAscii().data() ) );
    }

    void mousePressEvent( QMouseEvent* event )  {
        int button = 0;
        switch(event->button())
        {
            case(Qt::LeftButton): button = 1; break;
            case(Qt::MidButton): button = 2; break;
            case(Qt::RightButton): button = 3; break;
            case(Qt::NoButton): button = 0; break;
            default: button = 0; break;
        }
        gw_->getEventQueue()->mouseButtonPress(event->x(), event->y(), button);
    }

    void mouseReleaseEvent( QMouseEvent* event ) {
        int button = 0;
        switch(event->button())
        {
            case(Qt::LeftButton): button = 1; break;
            case(Qt::MidButton): button = 2; break;
            case(Qt::RightButton): button = 3; break;
            case(Qt::NoButton): button = 0; break;
            default: button = 0; break;
        }
        gw_->getEventQueue()->mouseButtonRelease(event->x(), event->y(), button);
    }

    void mouseMoveEvent( QMouseEvent* event ) {
        gw_->getEventQueue()->mouseMotion(event->x(), event->y());
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
        viewer_.setSceneData( scene );
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
                viewer_.getCamera()->setClearColor( osg::Vec4(
                                                                  rgba[ 0 ].toFloat(),
                                                                  rgba[ 1 ].toFloat(),
                                                                  rgba[ 2 ].toFloat(),
                                                                  rgba[ 3 ].toFloat() ) );
            }
        }
    }
private:
    osg::ref_ptr< osgViewer::GraphicsWindowEmbedded > gw_;
    osgViewer::Viewer viewer_;
    QTimer timer_;
    QString name_, version_, author_;
};

Q_EXPORT_PLUGIN2( osgglviewer, OSGViewerPlugin )
