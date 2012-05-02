#pragma once
//#SRCHEADER

#include <QObject>
#include <QtPlugin>
#include <QGraphicsView>
#include <QtOpenGL/QGLWidget>
#include <QResizeEvent>
#include "openglscene.h"


struct IDummy {};
Q_DECLARE_INTERFACE(IDummy,"dummy")

class GLViewPlugin : public QGraphicsView, public IDummy {
    Q_OBJECT
    Q_INTERFACES( IDummy )
public:
    GLViewPlugin() { 
        setStyleSheet( "QGraphicsView { border-style: none; }" );
        setWindowFlags( Qt::FramelessWindowHint );
        setViewport( new QGLWidget( QGLFormat(QGL::SampleBuffers | QGL::AccumBuffer | QGL::AlphaChannel), this, 0 ) );
        setViewportUpdateMode( QGraphicsView::FullViewportUpdate );
        setScene( new OpenGLScene );
    }
protected:
    void resizeEvent( QResizeEvent* event ) {
        if ( scene() ) {
            scene()->setSceneRect(
                        QRect( QPoint( 0, 0 ), event->size() ) );
            
        }
        QGraphicsView::resizeEvent( event );
    }
};

Q_EXPORT_PLUGIN2( glgview, GLViewPlugin )

