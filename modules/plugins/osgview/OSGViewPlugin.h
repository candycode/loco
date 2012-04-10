#pragma once
//#SRCHEADER

#include <QObject>
#include <QtPlugin>
#include <QGraphicsView>
#include <QtWebKit/QGraphicsWebView>
#include <QtWebKit/QWebHitTestResult>
#include <QtWebKit/QWebElement>
#include <QtOpenGL/QGLWidget>
#include <QGraphicsSceneMouseEvent>

#include <QApplication>
#include <QDir>

#include <osgGA/TrackballManipulator>

#include "QOSGGraphicsScene.h"

#include <LocoObject.h>


class TransparentGraphicsWebView : public QGraphicsWebView {
private:
    bool HandleEvent( QGraphicsSceneMouseEvent* ev ) const {
        QWebHitTestResult tr = this->page()->currentFrame()
            ->hitTestContent( ev->scenePos().toPoint() );
        
        
        if( tr.element().tagName() == "HTML" ) {
            
            return false; 
        }
        return true;    
    }
protected:
    void mousePressEvent( QGraphicsSceneMouseEvent * ev ) {
        if( HandleEvent( ev ) ) QGraphicsWebView::mousePressEvent( ev );
        else ev->setAccepted( false );
    }
    void mouseReleaseEvent( QGraphicsSceneMouseEvent * ev ) {
        if( HandleEvent( ev ) ) {
            QGraphicsWebView::mouseReleaseEvent( ev );
        }
        else ev->setAccepted( false );
        
    }
    void mouseMoveEvent( QGraphicsSceneMouseEvent * ev ) {
        QPointF p = mapFromScene( ev->scenePos().x(), ev->scenePos().y() );
        std::cout << "VIEW EVENT MOVE " << p.x() << " " << p.y() << std::endl;
        if( HandleEvent( ev ) ) QGraphicsWebView::mouseMoveEvent( ev );
        else ev->setAccepted( false );
    }
    void mouseDoubleClickEvent( QGraphicsSceneMouseEvent * ev ) {
        if( HandleEvent( ev ) ) QGraphicsWebView::mouseDoubleClickEvent( ev );
        else ev->setAccepted( false );
    }

};


struct IDummy {};
Q_DECLARE_INTERFACE(IDummy,"dummy")

class OSGViewPlugin : public QGraphicsView, public IDummy {
    Q_OBJECT
    Q_INTERFACES( IDummy )
public:
    OSGViewPlugin() :
        osgscene_( new osg::QOSGScene ),
                      webView_( new TransparentGraphicsWebView ) {

        setViewport( new QGLWidget( QGLFormat(QGL::SampleBuffers | QGL::AccumBuffer | QGL::AlphaChannel), this, 0 ) );
        setViewportUpdateMode( QGraphicsView::FullViewportUpdate );
        setScene( osgscene_ );
        //osgscene_->setContinuousUpdate( 50 );
        osgscene_->setCameraManipulator( new osgGA::TrackballManipulator );
        webView_->setCacheMode( QGraphicsItem::DeviceCoordinateCache ); //without this transparency doensn't work
        //wv->setAcceptedMouseButtons(0);
        //webView_->resize( 1024, 768 );
        webView_->settings()->setAttribute( QWebSettings::PluginsEnabled, true );
        webView_->settings()->setAttribute( QWebSettings::AcceleratedCompositingEnabled, true );
        webView_->settings()->setAttribute( QWebSettings::DeveloperExtrasEnabled, true );
        webView_->settings()->setAttribute( QWebSettings::JavascriptCanOpenWindows, true );
        webView_->settings()->setAttribute( QWebSettings::SpatialNavigationEnabled, true );
        webView_->settings()->setAttribute( QWebSettings::LocalContentCanAccessFileUrls, true );
        webView_->settings()->setAttribute( QWebSettings::LocalContentCanAccessRemoteUrls, true );
        webView_->settings()->setAttribute( QWebSettings::SiteSpecificQuirksEnabled, true );
//      webView_->settings()->setAttribute( QWebSettings::WebGLEnabled, true );
        webView_->setAcceptHoverEvents(true);
        osgscene_->addItem( webView_ );
    }
protected:
    void resizeEvent( QResizeEvent* event ) {
        if ( scene() ) {
            scene()->setSceneRect(
                        QRect( QPoint( 0, 0 ), event->size() ) );
            
        }
        webView_->resize( event->size().width(), event->size().height() );
        osgscene_->Resize( event->size().width(), event->size().height() );
        QGraphicsView::resizeEvent( event );
    }
public slots:
    void setOpacity( double opacity ) { webView_->setOpacity( opacity ); }
    void loadScene( const QString& path ) {
        osgscene_->LoadScene( path );
    }
    void loadPage( QString url, bool seethrough = false ) {
        if( !url.contains( "://" ) ) {
#ifdef Q_OS_WIN 
            url = "file:///" + QDir().absoluteFilePath( url );
#else
            url = "file://" + QDir().absoluteFilePath( url );
#endif
        }
        if( seethrough ) {
            QPalette palette = QApplication::palette();
            palette.setBrush(QPalette::Base, QColor::fromRgbF(0, 0, 0, 0));
            webView_->setPage( new QWebPage );
            webView_->page()->setPalette(palette);
        }
        webView_->load( QUrl( url ) );
    }
    void setContinuousUpdate( int ms ) { osgscene_->setContinuousUpdate( ms ); }
    void show( int width = 0, int height = 0 ) { 
        if( width > 0 && height > 0 ) resize( width, height );
        QGraphicsView::show();
    }

private:
    osg::QOSGScene* osgscene_;
    TransparentGraphicsWebView* webView_;

};

Q_EXPORT_PLUGIN2( osgview, OSGViewPlugin )

