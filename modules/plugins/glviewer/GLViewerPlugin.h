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
#if QT_VERSION >= 0x040800
        QGLFormat glFormat;
        glFormat.setVersion( 2, 0 );
        glFormat.setProfile( QGLFormat::CompatibilityProfile ); // Requires >=Qt-4.8.0
        glFormat.setSampleBuffers( true );
        setViewport( new QGLWidget( glFormat ) );
#else
        setViewport( new QGLWidget( QGLFormat(QGL::SampleBuffers | QGL::AccumBuffer | QGL::AlphaChannel) ) );
#endif
        setViewportUpdateMode( QGraphicsView::FullViewportUpdate );
        glScene = new OpenGLScene;
        setScene( glScene );
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
private:
    OpenGLScene* glScene;
};

Q_EXPORT_PLUGIN2( locoviewer, GLViewerPlugin )
