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

#include <QWidget>
#include <QGraphicsScene>

class QPainter;
class QRect;
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneDragDropEvent>
#include <QFocusEvent>
#include <QGraphicsSceneHelpEvent>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsItem>

#include "LocoObject.h"
#include "LocoWrappedWidget.h"
namespace loco {

class GraphicsSceneProxy : public QGraphicsScene {
    Q_OBJECT
protected:
    void drawBackground( QPainter* painter, const QRectF& rect ) {
        emit backDraw( painter, rect );
    }
    void drawForeground( QPainter* painter, const QRectF& rect ) {
        emit frontDraw( painter, rect );
    }
    void contextMenuEvent( QGraphicsSceneContextMenuEvent* event ) {
        QGraphicsScene::contextMenuEvent( event );
        if( event->isAccepted() ) return;
        emit contextMenu( event );
    } 
    void dragEnterEvent( QGraphicsSceneDragDropEvent* event ) {
        QGraphicsScene::dragEnterEvent( event );
        if( event->isAccepted() ) return;
        emit dragEnter( event );
    }
    void dragLeaveEvent( QGraphicsSceneDragDropEvent* event ) {
        QGraphicsScene::dragLeaveEvent( event );
        if( event->isAccepted() ) return;
        emit dragLeave( event );   
    }
    void dragMoveEvent( QGraphicsSceneDragDropEvent* event ) {
        QGraphicsScene::dragMoveEvent( event );
        if( event->isAccepted() ) return;
        emit dragMove( event );
    }
    void dropEvent( QGraphicsSceneDragDropEvent* event ) {
        QGraphicsScene::dropEvent( event );
        if( event->isAccepted() ) return;
        emit drop( event );
    }
    void focusInEvent( QFocusEvent* event ) {
        QGraphicsScene::focusInEvent( event );
        if( event->isAccepted() ) return;
        emit focusIn( event );
    }
    void focusOutEvent( QFocusEvent* event ) {
        QGraphicsScene::focusOutEvent( event );
        if( event->isAccepted() ) return;
        emit focusOut( event );
    }
    void helpEvent( QGraphicsSceneHelpEvent* event ) {
        QGraphicsScene::helpEvent( event );
        if( event->isAccepted() ) return;
        emit help( event );
    }
    void inputMethodEvent( QInputMethodEvent* event ) {
        QGraphicsScene::inputMethodEvent( event );
        if( event->isAccepted() ) return;
        emit inputMethod( event );
    }
    void keyPressEvent( QKeyEvent* event ) {
        QGraphicsScene::keyPressEvent( event );
        if( event->isAccepted() ) return;
        emit keyPress( event );
    }
    void keyReleaseEvent( QKeyEvent* event ) {
        QGraphicsScene::keyReleaseEvent( event );
        if( event->isAccepted() ) return;
        emit keyRelease( event );
    }
    void mouseDoubleClickEvent( QGraphicsSceneMouseEvent* event ) {
        QGraphicsScene::mouseDoubleClickEvent( event );
        if( event->isAccepted() ) return;
        emit mouseDoubleClick( event );
    }
    void mouseMoveEvent( QGraphicsSceneMouseEvent* event ) {
        QGraphicsScene::mouseMoveEvent( event );
        if( event->isAccepted() ) return;
        emit mouseMove( event );
    }
    void mousePressEvent( QGraphicsSceneMouseEvent* event ) {
        QGraphicsScene::mousePressEvent( event );
        if( event->isAccepted() ) return;
        emit mousePress( event );
    }
    void mouseReleaseEvent( QGraphicsSceneMouseEvent* event ) {
        QGraphicsScene::mouseReleaseEvent( event );
        if( event->isAccepted() ) return;
        emit mouseRelease( event );
    }
    void wheelEvent( QGraphicsSceneWheelEvent* event ) {
        QGraphicsScene::wheelEvent( event );
        if( event->isAccepted() ) return;
        emit wheel( event );
    }
public slots:
    void redraw() { update(); }
signals:
    void contextMenu( QGraphicsSceneContextMenuEvent* contextMenuEvent );
    void dragEnter( QGraphicsSceneDragDropEvent* event );
    void dragLeave( QGraphicsSceneDragDropEvent* event );
    void dragMove( QGraphicsSceneDragDropEvent* event );
    void backDraw( QPainter* painter, const QRectF& rect );
    void frontDraw( QPainter* painter, const QRectF& rect );
    void drop( QGraphicsSceneDragDropEvent* event );
    void focusIn( QFocusEvent* focusEvent );
    void focusOut( QFocusEvent* focusEvent );
    void help( QGraphicsSceneHelpEvent* helpEvent );
    void inputMethod( QInputMethodEvent* event );
    void keyPress( QKeyEvent* keyEvent );
    void keyRelease( QKeyEvent* keyEvent );
    void mouseDoubleClick( QGraphicsSceneMouseEvent* mouseEvent );
    void mouseMove( QGraphicsSceneMouseEvent* mouseEvent );
    void mousePress( QGraphicsSceneMouseEvent* mouseEvent );
    void mouseRelease( QGraphicsSceneMouseEvent* mouseEvent );
    void wheel( QGraphicsSceneWheelEvent* wheelEvent );
};


class GraphicsScene : public Object {
    Q_OBJECT
    Q_PROPERTY( QObject* graphicsScene READ GetQGraphicsScene WRITE SetQGraphicsScene )
public:
    GraphicsScene( QObject* graphicsScene = 0, //need to use QObject to allow access from Javascript
            Context* c = 0, 
            const QString& n = "LocoGraphicsScene",
            const QString& type = "", 
            const QString& module = "",
            bool destroyable = false,
            ObjectInfo* objInfo = 0 ) : 
                Object( c, n, type, module, destroyable, objInfo ),
                graphicsScene_( qobject_cast< QGraphicsScene* >( graphicsScene_ ) ) {
    }
    void SetQGraphicsScene( QObject* gs ) {
        if( !qobject_cast< QGraphicsScene* >( gs ) ) error( "QGraphicsScene type expected" );
        else { 
            if( graphicsScene_ ) graphicsScene_->deleteLater();
            graphicsScene_ = qobject_cast< QGraphicsScene* >( gs ); 
        }
    }
    QObject* GetQGraphicsScene() const { return graphicsScene_; }
    ~GraphicsScene() {
        if( graphicsScene_ ) graphicsScene_->deleteLater();
    }
public slots:
    void addWidget( QObject* widget, const QVariantMap& flags = QVariantMap() ) {
        QWidget* w = qobject_cast< QWidget* >( widget );
        if( w == 0 && qobject_cast< ::loco::WrappedWidget* >( widget ) ) {
            w = qobject_cast< ::loco::WrappedWidget* >( widget )->Widget();    
        } else error( "QWidget or loco::WrappedWidget type expected" );
        graphicsScene_->addWidget( w ) != 0;
    }
    void addItem( QObject* item ) {
        if( qobject_cast< QGraphicsItem* >( item ) == 0 ) error( "QGraphicsItem type expected" );
        graphicsScene_->addItem( qobject_cast< QGraphicsItem* >( item ) );    
    }
    void update() { graphicsScene_->update(); }
private:
    QGraphicsScene* graphicsScene_;
};
}

