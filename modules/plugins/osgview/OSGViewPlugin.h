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
//==============================================================================
//WARNING!!!!
// IN ORDER FOR THE EVENT EXTENT TO BE CORRECT osgViewer::Viewer needs to have
// the following line added in the eventTraversal method with OSG > 3.1(if they
// do not change the code again)
// eventState->setInputRange( viewport->x(), viewport->y(),
//      viewport->x() + viewport->width(), viewport->y() + viewport->height());
// Gist: https://gist.github.com/9e9e1c8e3d9b08c4e2ef
//==============================================================================
#include <QObject>
#include <QtPlugin>
#include <QGraphicsView>
#include <QtWebKit/QGraphicsWebView>
#include <QtWebKit/QWebHitTestResult>
#include <QtWebKit/QWebElement>
#include <QtOpenGL/QGLWidget>
#include <QGraphicsSceneMouseEvent>
#include <QList>
#include <QVariantMap>

#include <QApplication>
#include <QDir>

#include <osgGA/TrackballManipulator>

#include "QOSGGraphicsScene.h"


class TransparentGraphicsWebView : public QGraphicsWebView {
private:
    bool HandleEvent( QGraphicsSceneMouseEvent* ev ) {
        QWebHitTestResult tr = this->page()->currentFrame()
            ->hitTestContent( ev->scenePos().toPoint() );
        //std::cout << tr.element().tagName().toStdString() << std::endl;    
        if( tr.element().tagName() == "HTML" ) {
            focus_ = false;
            return false; 
        }
        focus_ = true;
        return true;    
    }
protected:
    void contextMenuEvent ( QGraphicsSceneContextMenuEvent* ev ) {
        ev->setAccepted( false );
    }
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
        if( focus_ ) QGraphicsWebView::mouseMoveEvent( ev );
        else ev->setAccepted( false );
    }
    void mouseDoubleClickEvent( QGraphicsSceneMouseEvent * ev ) {
        if( HandleEvent( ev ) ) QGraphicsWebView::mouseDoubleClickEvent( ev );
        else ev->setAccepted( false );
    }
    void wheelEvent( QGraphicsSceneWheelEvent* ev ) {
        //if( HandleEvent( ev ) ) QGraphicsWebView::wheelEvent( ev );
        ev->setAccepted( false );
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
    Q_PROPERTY( QString jsinit READ GetJSInitCode WRITE SetInitJSCode )
    Q_INTERFACES( IDummy )
public:
    OSGViewPlugin() :
        osgscene_( new osg::QOSGScene ),
                      webView_( new TransparentGraphicsWebView ) {
        setRenderHint( QPainter::Antialiasing );
        setStyleSheet( "QGraphicsView { border-style: none; }" );
        setWindowFlags( Qt::FramelessWindowHint );
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
    void SetInitJSCode( const QString& code ) {
        jsInitCode_ = code;
    }
    const QString& GetJSInitCode() const { return jsInitCode_; } 
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
        webView_->page()->mainFrame()->evaluateJavaScript( jsInitCode_ );
    }
public slots:
    void setBkColor( QVariantMap rgb ) {
        float r = 0;
        float g = 0;
        float b = 0;
        if( rgb.contains( "r" ) ) r = rgb[ "r" ].toInt() / 255.0f;
        if( rgb.contains( "g" ) ) g = rgb[ "g" ].toInt() / 255.0f;
        if( rgb.contains( "b" ) ) b = rgb[ "b" ].toInt() / 255.0f;
        osgscene_->SetBkColor( r, g, b );
    }
    void fullScreen() { showFullScreen(); }
    void setHUDOpacity( double opacity ) { webView_->setOpacity( opacity ); }
    void setHUDGeometry( qreal x, qreal y, int w, int h ) {
        webView_->setGeometry( QRectF( x, y, w, h ) );
    }
    void loadScene( const QString& path, const QString& options = QString() ) {
        osgscene_->LoadScene( path, options );
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
        webView_->page()->mainFrame()->setScrollBarPolicy( Qt::Vertical, Qt::ScrollBarAlwaysOff );
        if( seethrough ) {
            QPalette palette = webView_->palette();
            palette.setBrush(QPalette::Base, Qt::transparent);
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
    void setLighting( bool on ) { osgscene_->Lighting( on ); }
    void setTexture( bool on ) { osgscene_->Texture( on ); }
    QVariant eval( const QString& code ) {
        return webView_->page()->mainFrame()->evaluateJavaScript( code );
    }
private:
    void AddJSObject( const QString& name, QObject* obj ) {
        webView_->page()->mainFrame()->addToJavaScriptWindowObject( name, obj );
    }
private:
    osg::QOSGScene* osgscene_;
    TransparentGraphicsWebView* webView_;
    QList< JSObject > jsObjects_;
    QString jsInitCode_;
};

Q_EXPORT_PLUGIN2( osgview, OSGViewPlugin )

