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
#include <iostream>
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
#include <QGraphicsWidget>

#include "LocoObject.h"
#include "LocoWrappedWidget.h"
#include "LocoWrappedGraphicsWidget.h"
#include <iostream>
namespace loco {

class GraphicsSceneProxy : public QGraphicsScene {
    Q_OBJECT
public:
    GraphicsSceneProxy() : handled_( false ) {}
protected:
    void drawBackground( QPainter* painter, const QRectF& rect ) {
        emit backDraw();// painter, rect );
    }
    /*void drawForeground( QPainter* painter, const QRectF& rect ) {
        emit frontDraw( painter, rect );
    }*/
    void contextMenuEvent( QGraphicsSceneContextMenuEvent* event ) {
        QGraphicsScene::contextMenuEvent( event );
        if( event->isAccepted() ) return;
        emit contextMenu( handled_ );
        if( handled_ ) event->accept();
    } 
    void dragEnterEvent( QGraphicsSceneDragDropEvent* event ) {
        QGraphicsScene::dragEnterEvent( event );
        if( event->isAccepted() ) return;
        emit dragEnter( event->pos().x(), event->pos().y(), event->mimeData(), handled_ );
        if( handled_ ) event->accept();
    }
    void dragLeaveEvent( QGraphicsSceneDragDropEvent* event ) {
        QGraphicsScene::dragLeaveEvent( event );
        if( event->isAccepted() ) return;
        emit dragLeave( event->pos().x(), event->pos().y(), event->mimeData(), handled_ );
        if( handled_ ) event->accept();   
    }
    void dragMoveEvent( QGraphicsSceneDragDropEvent* event ) {
        QGraphicsScene::dragMoveEvent( event );
        if( event->isAccepted() ) return;
        emit dragMove( event->pos().x(), event->pos().y(), event->mimeData(), handled_ );
        if( handled_ ) event->accept();
    }
    void dropEvent( QGraphicsSceneDragDropEvent* event ) {
        QGraphicsScene::dropEvent( event );
        if( event->isAccepted() ) return;
        emit drop( event->pos().x(), event->pos().y(), event->mimeData(), handled_ );
        if( handled_ ) event->accept();
    }
    void focusInEvent( QFocusEvent* event ) {
        QGraphicsScene::focusInEvent( event );
        if( event->isAccepted() ) return;
        emit focusIn( handled_ );
        if( handled_ ) event->accept();
    }
    void focusOutEvent( QFocusEvent* event ) {
        QGraphicsScene::focusOutEvent( event );
        if( event->isAccepted() ) return;
        emit focusOut( handled_ );
        if( handled_ ) event->accept();
    }
    void helpEvent( QGraphicsSceneHelpEvent* event ) {
        QGraphicsScene::helpEvent( event );
        if( event->isAccepted() ) return;
        emit help( handled_ );
        if( handled_ ) event->accept();
    }
    //void inputMethodEvent( QInputMethodEvent* event ) {
    //    QGraphicsScene::inputMethodEvent( event );
    //    if( event->isAccepted() ) return;
    //    emit inputMethod( event, handled_ );
    //    if( handled_ ) event->accept();
    //}
    void keyPressEvent( QKeyEvent* event ) {
        QGraphicsScene::keyPressEvent( event );
        if( event->isAccepted() ) return;
        emit keyPress( event, handled_ );
        if( handled_ ) event->accept();
    }
    void keyReleaseEvent( QKeyEvent* event ) {
        QGraphicsScene::keyReleaseEvent( event );
        if( event->isAccepted() ) return;
        emit keyRelease( event, handled_ );
        if( handled_ ) event->accept();
    }
    void mouseDoubleClickEvent( QGraphicsSceneMouseEvent* event ) {
        QGraphicsScene::mouseDoubleClickEvent( event );
        if( event->isAccepted() ) return;
        emit mouseDoubleClick( event->button(),
                               event->buttons(),
                               event->modifiers(),
                               event->scenePos().x(),
                               event->scenePos().y(),
                               handled_ );
        if( handled_ ) event->accept();
    }
    void mouseMoveEvent( QGraphicsSceneMouseEvent* event ) {
        QGraphicsScene::mouseMoveEvent( event );
        if( event->isAccepted() ) return;
        emit mouseMove( event->button(),
                        event->buttons(),
                        event->modifiers(),
                        event->lastScenePos().x(),
                        event->lastScenePos().y(),
                        event->scenePos().x(),
                        event->scenePos().y(),
                        handled_ );
        if( handled_ ) event->accept();
    }
    void mousePressEvent( QGraphicsSceneMouseEvent* event ) {
        QGraphicsScene::mousePressEvent( event );
        if( event->isAccepted() ) return;
        emit mousePress( event->button(),
                         event->buttons(),
                         event->modifiers(),
                         event->scenePos().x(),
                         event->scenePos().y(),
                         handled_ );
        if( handled_ ) event->accept();
    }
    void mouseReleaseEvent( QGraphicsSceneMouseEvent* event ) {
        QGraphicsScene::mouseReleaseEvent( event );
        if( event->isAccepted() ) return;
        emit mouseRelease( event->button(),
                           event->buttons(),
                           event->modifiers(),
                           event->scenePos().x(),
                           event->scenePos().y(),
                           handled_ );
        if( handled_ ) event->accept(); 
    }
    void wheelEvent( QGraphicsSceneWheelEvent* event ) {
        QGraphicsScene::wheelEvent( event );
        if( event->isAccepted() ) return;
        emit wheel( event->delta(), event->orientation(), event->modifiers(), handled_ );
        if( handled_ ) event->accept();
    }
public slots:
    void redraw() { update(); }
    void add( QObject* item ) {
        if( qobject_cast< QGraphicsItem* >( item ) != 0 ) {
            addItem( qobject_cast< QGraphicsItem* >( item ) );
        } else if( qobject_cast< QGraphicsWidget* >( item ) != 0 ) {
            addItem( qobject_cast< QGraphicsWidget* >( item ) );
        } else if( qobject_cast< QWidget* >( item ) != 0 ) {   
            addWidget( qobject_cast< QWidget* >( item ) );
        } else if( qobject_cast< WrappedWidget* >( item ) ) {
            add( qobject_cast< WrappedWidget* >( item )->Widget() );
        } else if( qobject_cast< WrappedGraphicsWidget* >( item ) ) {
            add( qobject_cast< WrappedGraphicsWidget* >( item )->Widget() );
        } else emit error( "Wrong object type, supported types are:\n"
                           "QWidget, QGraphicsWidget, QGraphicsItem, loco::WrappedWidget,"
                           "  loco::WrappedGraphicsWidget" );       
    }
signals:
    void error( const QString& ); 
    void contextMenu( bool& handled_ );
    void dragEnter( int x, int y, const QMimeData* data, bool& handled_ );
    void dragLeave( int x, int y, const QMimeData* data, bool& handled_ );
    void dragMove( int x, int y, const QMimeData* data, bool& handled_  );
    void backDraw();// QPainter* painter, const QRectF& rect );
    void frontDraw( QPainter* painter, const QRectF& rect );
    void drop( int x, int y, const QMimeData* data, bool& handled_ );
    void focusIn( bool& handled_ );
    void focusOut( bool& handled_ );
    void help( bool& handled_ );
    //void inputMethod( QInputMethodEvent* event );
    void keyPress( QKeyEvent* keyEvent, bool& handled_ );
    void keyRelease( QKeyEvent* keyEvent, bool& handled_ );
    void mouseDoubleClick( Qt::MouseButton button, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers, int x, int y, bool& handled_  );
    void mouseMove( Qt::MouseButton button, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers, int prevX, int prevY, int x, int y, bool& handled_ );
    void mousePress( Qt::MouseButton button, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers, int x, int y, bool& handled_ );
    void mouseRelease( Qt::MouseButton button, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers, int x, int y, bool& handled_ );
    void wheel( int delta, Qt::Orientation orientation, Qt::KeyboardModifiers modifiers, bool& handled_ );
private:
    mutable bool handled_;

};

/*
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
    void add( QObject* item ) {
        if( qobject_cast< QGraphicsItem* >( item ) != 0 ) {
            std::cout << "Added item" << std::endl;   
            graphicsScene_->addItem( qobject_cast< QGraphicsItem* >( item ) );
        } else if( qobject_cast< QGraphicsWidget* >( item ) != 0 ) {
            graphicsScene_->addItem( qobject_cast< QGraphicsWidget* >( item ) );
        } else if( qobject_cast< QWidget* >( item ) != 0 ) {   
            graphicsScene_->addWidget( qobject_cast< QWidget* >( item ) );
        } else if( qobject_cast< WrappedWidget* >( item ) ) {
            add( qobject_cast< WrappedWidget* >( item )->Widget() );
        } else if( qobject_cast< WrappedGraphicsWidget* >( item ) ) {
            add( qobject_cast< WrappedGraphicsWidget* >( item )->Widget() );
        } else error( "Wrong object type, supported types are:\n"
                      "QWidget, QGraphicsWidget, QGraphicsItem, loco::WrappedWidget,"
                      "  loco::WrappedGraphicsWidget" );       
    }
    void update() { graphicsScene_->update(); }
private:
    QGraphicsScene* graphicsScene_;
};*/
}

