#pragma once
//#SRCHEADER

#include <QObject>
#include <QtPlugin>
#include <QGraphicsView>
#include <QtOpenGL/QGLWidget>
#include <QResizeEvent>
#include <QtWebKit/QGraphicsWebView>
#include <QtWebKit/QWebFrame>
#include <QDir>
#include "openglscene.h"


struct IDummy {};
Q_DECLARE_INTERFACE(IDummy,"dummy")

class GLViewPlugin : public QGraphicsView, public IDummy {
    Q_OBJECT
    Q_INTERFACES( IDummy )
public:
    GLViewPlugin() : scene_( new OpenGLScene ),
                     webView_( 0 ) {
        setStyleSheet( "QGraphicsView { border-style: none; }" );
        setRenderHint( QPainter::Antialiasing );
        setStyleSheet( "QGraphicsView { border-style: none; }" );
        setWindowFlags( Qt::FramelessWindowHint );
        setViewport( new QGLWidget( QGLFormat(QGL::SampleBuffers | QGL::AccumBuffer | QGL::AlphaChannel), this, 0 ) );
        setViewportUpdateMode( QGraphicsView::FullViewportUpdate );
        setScene( scene_ );
    }
protected:
    void resizeEvent( QResizeEvent* event ) {
        if ( scene() ) {
            scene()->setSceneRect(
                        QRect( QPoint( 0, 0 ), event->size() ) );
            
        }
        QGraphicsView::resizeEvent( event );
    }
public slots:
    void loadPage( QString url, bool seethrough = false ) {
        if( !url.contains( "://" ) ) {
#ifdef Q_OS_WIN 
            url = "file:///" + QDir().absoluteFilePath( url );
#else
            url = "file://" + QDir().absoluteFilePath( url );
#endif
        }
        if( webView_ == 0 ) {
            webView_ = new QGraphicsWebView;
            webView_->setCacheMode( QGraphicsItem::DeviceCoordinateCache ); //without this transparency doensn't work
            webView_->settings()->setAttribute( QWebSettings::AcceleratedCompositingEnabled, true );
            webView_->settings()->setAttribute( QWebSettings::SpatialNavigationEnabled, true );
        }
        webView_->settings()->setAttribute( QWebSettings::LocalContentCanAccessFileUrls, true );
        webView_->settings()->setAttribute( QWebSettings::LocalContentCanAccessRemoteUrls, true );
        scene_->addItem( webView_ );
        webView_->page()->mainFrame()->setScrollBarPolicy( Qt::Horizontal, Qt::ScrollBarAlwaysOff );
        webView_->page()->mainFrame()->setScrollBarPolicy( Qt::Vertical, Qt::ScrollBarAlwaysOff );
        if( seethrough ) {
            QPalette palette = webView_->palette();
            palette.setBrush(QPalette::Base, Qt::transparent);
            webView_->page()->setPalette(palette);
            webView_->setAttribute( Qt::WA_OpaquePaintEvent, false );
        }
        webView_->load( QUrl( url ) );
    }
private:
    QGraphicsScene* scene_;
    QGraphicsWebView* webView_;
};

Q_EXPORT_PLUGIN2( glgview, GLViewPlugin )

