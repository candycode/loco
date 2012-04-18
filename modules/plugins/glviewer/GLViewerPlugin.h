#pragma once


#include <QGraphicsView>
#include <QGLWidget>
#include <QResizeEvent>
#include <QString>
#include <QtPlugin>
#include <QVBoxLayout>

#include "openglscene.h"

struct IDummy {};
Q_DECLARE_INTERFACE(IDummy,"dummy")

class GLViewerPlugin : public QGraphicsView, public IDummy {
    Q_OBJECT
    Q_INTERFACES( IDummy )
public:
    GLViewerPlugin()  {
        setWindowTitle( tr( "3D Model Viewer") );
        //setViewport( new QGLWidget( QGLFormat(QGL::SampleBuffers | QGL::AccumBuffer | QGL::AlphaChannel) ) );
        glScene = new OpenGLScene;
        setScene( glScene );
        setViewport( new QGLWidget( QGLFormat( QGL::SampleBuffers ), this ) );
        setViewportUpdateMode( QGraphicsView::FullViewportUpdate );
    //    setViewport( new QGLWidget( QGLFormat(QGL::SampleBuffers | QGL::AccumBuffer | QGL::AlphaChannel) ) );
    }
protected:
    void resizeEvent(QResizeEvent *event) {
        if( scene() ) {
            scene()->setSceneRect( QRect( QPoint( 0,  0 ), event->size() ) );
        }
        QGraphicsView::resizeEvent( event );
    }
public slots:
    void load( const QString& fpath ) { glScene->loadModel( fpath ); }
    void show() { QWidget::show(); }
private:
    OpenGLScene* glScene;
};

Q_EXPORT_PLUGIN2( locoviewer, GLViewerPlugin )
