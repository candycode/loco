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

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QtOpenGL/QGLWidget>
#include <QVariantMap>
#include <QSize>

#include "LocoWrappedWidget.h"
#include "LocoQGLFormat.h"
#include "LocoGraphicsScene.h"

namespace loco {

class LGraphicsView : public QGraphicsView {
    Q_OBJECT
public:
    LGraphicsView() : sceneAutoResize_( true ) {}
    void SetSceneAutoResize( bool on ) { sceneAutoResize_ = on; }
    bool GetSceneAutoResize() const { return sceneAutoResize_; }
protected:
    void resizeEvent( QResizeEvent* ev ) {
        if ( sceneAutoResize_ && scene() ) {
            scene()->setSceneRect(
                        QRect( QPoint( 0, 0 ), ev->size() ) );

        }
        emit resized( ev->size().width(), ev->size().height() );
        QGraphicsView::resizeEvent( ev );
    }
signals:
    void resized( int width, int height );
private:
    bool sceneAutoResize_;
};

class GraphicsView : public WrappedWidget {
    Q_OBJECT
    Q_PROPERTY( QObject* scene READ GetScene WRITE SetScene )
    Q_PROPERTY( QObject* viewport READ GetViewport WRITE SetViewport )
    Q_PROPERTY( bool sceneAutoResize READ GetSceneAutoResize WRITE SetSceneAutoResize )
public:
    GraphicsView() : WrappedWidget( 0, "LocoGraphicsView", "Loco/GUI/GraphicsView" ),
        graphicsView_( new LGraphicsView() ) {
        // base class instance is constructed before current instance, it is therefore
        // not possible to connect signals in base class constructor since Widget() method
        // must return a pointer which is set within this constructor
        //WrappedWidget::ConnectSignals();
        connect( graphicsView_, SIGNAL( resized( int, int ) ), this, SIGNAL( resized( int, int ) ) );
    }
    QWidget* Widget() { return graphicsView_; }
    virtual const QWidget* Widget() const { return graphicsView_; }
    QGraphicsView* GetQGraphicsView() const { return graphicsView_; }
    void SetSceneAutoResize( bool on ) { graphicsView_->SetSceneAutoResize( on ); }
    bool GetSceneAutoResize() const { return graphicsView_->GetSceneAutoResize(); }
    ~GraphicsView() { if( graphicsView_ && graphicsView_->parent() == 0 ) graphicsView_->deleteLater(); }
public:
    void SetScene( QObject* obj ) {
        QGraphicsScene* gs = 0;
        if( qobject_cast< QGraphicsScene* >( obj ) != 0 ) gs = qobject_cast< QGraphicsScene* >( obj );
        else if( qobject_cast< ::loco::GraphicsScene* >( obj ) != 0 ) {
            gs = qobject_cast< QGraphicsScene* >( 
                     qobject_cast< ::loco::GraphicsScene* >( obj )->GetQGraphicsScene() );
        } 
        if( gs == 0 ) error( "Invalid object type: QGraphicsScene expected" );
        graphicsView_->setScene( gs );
    }
    QObject* GetScene() const { return graphicsView_->scene(); }
    void SetViewport( QObject* obj ) {
        if( qobject_cast< QWidget* >( obj ) == 0 ) {
            error( "Invalid object type: QWidget expected" );
        } else graphicsView_->setViewport( qobject_cast< QWidget* >( obj ) );
    }
    QObject* GetViewport() const { return graphicsView_->viewport(); }
    void SetViewportUpdateMode( const QString& um ) {
        if( um == "full" ) graphicsView_->setViewportUpdateMode( QGraphicsView::FullViewportUpdate );
        else if( um == "minimal" ) {
            graphicsView_->setViewportUpdateMode( QGraphicsView::MinimalViewportUpdate );
        } else if( um == "smart" ) {
            graphicsView_->setViewportUpdateMode( QGraphicsView::SmartViewportUpdate );
        } else if( um == "none" ) {
            graphicsView_->setViewportUpdateMode( QGraphicsView::NoViewportUpdate );
        } else error( "Unrecognized viewport update mode: " + um );
    }
signals:
    void resized( int width, int height );
public slots:
    void createOpenGLViewport( const QVariantMap& properties ) {
        //graphicsView_->setViewport( new QGLWidget( QGLFormat(QGL::SampleBuffers | QGL::AccumBuffer | QGL::AlphaChannel), graphicsView_, 0 ) );
        graphicsView_->setViewport( new QGLWidget( OpenGLFormat( properties ), graphicsView_, 0 ) );
    }
    QObject* createGraphicsSceneProxy() {
        SetScene( new GraphicsSceneProxy );
        return GetScene();
    }
    void setViewportUpdateMode( const QString& um ) { SetViewportUpdateMode( um ); }
private:
    LGraphicsView* graphicsView_;
};
}
