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
#include <QList>

#include <QApplication>
#include <QDir>

#include <osgGA/TrackballManipulator>

#include "QOSGGraphicsScene.h"

#include <LocoObject.h>


class TransparentGraphicsWebView : public QGraphicsWebView {
private:
    bool HandleEvent( QGraphicsSceneMouseEvent* ev ) {
        QWebHitTestResult tr = this->page()->currentFrame()
            ->hitTestContent( ev->scenePos().toPoint() );
        std::cout << tr.element().tagName().toStdString() << std::endl;    
        if( tr.element().tagName() == "HTML" ) {
            focus_ = false;
            return false; 
        }
        focus_ = true;
        return true;    
    }
protected:
    void keyPressEvent( QKeyEvent* event ) {
        if( focus_ ) {
            QGraphicsWebView::keyPressEvent( event );
        } else event->setAccepted( false );
    }
    void keyReleaseEvent( QKeyEvent* event ) {
        if( focus_ ) {
            QGraphicsWebView::keyReleaseEvent( event );
        } else event->setAccepted( false );
    }
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
public:
   TransparentGraphicsWebView() : focus_( true ) {}
private:
   bool focus_;

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
        osgscene_->setCameraManipulator( new osgGA::TrackballManipulator );
        webView_->setCacheMode( QGraphicsItem::DeviceCoordinateCache ); //without this transparency doensn't work
        webView_->settings()->setAttribute( QWebSettings::AcceleratedCompositingEnabled, true );
        webView_->settings()->setAttribute( QWebSettings::SpatialNavigationEnabled, true );
        webView_->settings()->setAttribute( QWebSettings::LocalContentCanAccessFileUrls, true );
        webView_->settings()->setAttribute( QWebSettings::LocalContentCanAccessRemoteUrls, true );
        osgscene_->addItem( webView_ );
        connect( webView_->page()->mainFrame(), SIGNAL( javaScriptWindowObjectCleared() ),
                 this, SLOT( AddJSObjects() ) );
    }
private:
    struct JSObject {
        QString name;
        QObject* obj;
        JSObject( const QString n, QObject* o ) : name( n ), obj( o ) {}
    };
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
private slots:
    void AddJSObjects() {
        foreach( JSObject js, jsObjects_ ) {
            AddJSObject( js.name, js.obj );
        }     
        AddJSObject( "osgviewer", this );
    }
public slots:
    void fullScreen() { showFullScreen(); }
    void setOpacity( double opacity ) { webView_->setOpacity( opacity ); }
    void loadScene( const QString& path ) {
        osgscene_->LoadScene( path );
    }
    void addJSObject( const QString& name, QObject* obj ) {
        jsObjects_.push_back( JSObject( name, obj ) );
    }
    void loadPage( QString url, bool seethrough = false ) {
        if( !url.contains( "://" ) ) {
#ifdef Q_OS_WIN 
            url = "file:///" + QDir().absoluteFilePath( url );
#else
            url = "file://" + QDir().absoluteFilePath( url );
#endif
        }
        webView_->page()->mainFrame()->setScrollBarPolicy( Qt::Horizontal, Qt::ScrollBarAlwaysOff );
        if( seethrough ) {
            QPalette palette = webView_->palette();
            palette.setBrush(QPalette::Base, Qt::transparent);
            //webView_->setPage( new QWebPage );
            webView_->page()->setPalette(palette);
            webView_->setAttribute( Qt::WA_OpaquePaintEvent, false );
        }
        webView_->load( QUrl( url ) );
    }
    void setContinuousUpdate( int ms ) { osgscene_->setContinuousUpdate( ms ); }
    void show( int width = 0, int height = 0 ) { 
        if( width > 0 && height > 0 ) resize( width, height );
        QGraphicsView::show();
    }
    void setWireframe() { osgscene_->Wireframe(); }
    void setPoint() { osgscene_->Point(); }
    void setSolid() { osgscene_->Solid(); }
private:
    void AddJSObject( const QString& name, QObject* obj ) {
        webView_->page()->mainFrame()->addToJavaScriptWindowObject( name, obj );
    }
private:
    osg::QOSGScene* osgscene_;
    TransparentGraphicsWebView* webView_;
    QList< JSObject > jsObjects_;
};

Q_EXPORT_PLUGIN2( osgview, OSGViewPlugin )

