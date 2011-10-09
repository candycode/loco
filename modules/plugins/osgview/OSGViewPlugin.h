#pragma once
//#SRCHEADER

#include <QObject>
#include <QtPlugin>
#include <QGraphicsView>
#include <QtWebKit/QGraphicsWebView>
#include <QtOpenGL/QGLWidget>

#include <QApplication>
#include <QDir>

#include <osgGA/TrackballManipulator>

#include "QOSGGraphicsScene.h"

#include <LocoObject.h>
struct IDummy {};
Q_DECLARE_INTERFACE(IDummy,"dummy")

class OSGViewPlugin : public QGraphicsView, public IDummy {
	Q_OBJECT
	Q_INTERFACES( IDummy )
public:
    OSGViewPlugin() :
		osgscene_( new osg::QOSGScene ),
                      webView_( new QGraphicsWebView ) {

        setViewport( new QGLWidget( QGLFormat(QGL::SampleBuffers | QGL::AccumBuffer | QGL::AlphaChannel) ) );
        setViewportUpdateMode( QGraphicsView::FullViewportUpdate );
        setScene( osgscene_ );
        osgscene_->setContinuousUpdate( 50 );
        osgscene_->setCameraManipulator( new osgGA::TrackballManipulator );
        webView_->setCacheMode( QGraphicsItem::DeviceCoordinateCache ); //without this transparency doensn't work
		//wv->setAcceptedMouseButtons(0);
		webView_->resize( 1024, 768 );
		webView_->settings()->setAttribute( QWebSettings::PluginsEnabled, true );
		webView_->settings()->setAttribute( QWebSettings::AcceleratedCompositingEnabled, true );
		webView_->settings()->setAttribute( QWebSettings::DeveloperExtrasEnabled, true );
		webView_->settings()->setAttribute( QWebSettings::JavascriptCanOpenWindows, true );
		webView_->settings()->setAttribute( QWebSettings::SpatialNavigationEnabled, true );
		webView_->settings()->setAttribute( QWebSettings::LocalContentCanAccessFileUrls, true );
		webView_->settings()->setAttribute( QWebSettings::WebGLEnabled, true );
	
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
		
        //QGraphicsView::resizeEvent( event );
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
	void show( int width = 0, int height = 0 ) { 
		if( width > 0 && height > 0 ) resize( width, height );
		QGraphicsView::show();
	}
private:
	osg::QOSGScene* osgscene_;
    QGraphicsWebView* webView_;

};

Q_EXPORT_PLUGIN2( loco_OSGViewPlugin, OSGViewPlugin )
