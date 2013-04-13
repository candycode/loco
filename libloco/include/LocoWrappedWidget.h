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
#include <QIcon>
#include <QBitmap>
#include <QtGui/QPixmap>
#include <QtGui/QPainter>

#include "LocoObject.h"
#include "LocoLayout.h"
#include "LocoWindowAttributes.h"

namespace loco {

//to be able to cast from QObject we need to pass a QObject derived object to qobject_cast 

class  WrappedWidget : public Object {
    Q_OBJECT
    Q_PROPERTY( bool fullScreen READ IsFullScreen )
    Q_PROPERTY( QObject* qwidget READ GetWidget )
public:
    WrappedWidget( Context* c = 0, 
                   const QString& n = "LocoWrappedWidget",
                   const QString& type = "", 
                   const QString& module = "",
                   bool destroyable = false,
                   ObjectInfo* objInfo = 0 ) : Object( c, n, type, module, destroyable, objInfo ) {}

    bool IsFullScreen() const { return Widget()->isFullScreen(); }
    QObject* GetWidget() { return Widget(); }
public:
    virtual QWidget* Widget()  = 0;
    virtual const QWidget* Widget() const = 0;
    // ownership of wrapped widget transferred to widget wrapped by parent WrappedWidget
    virtual void SetParentWidget( WrappedWidget* pw) {
        Widget()->setParent( pw->Widget() );
    }
    void SetLayout( QObject* ly ) {
        Layout* l = qobject_cast< Layout* >( ly );
        if( !l ) {
            error( "WrappedWidget::setLayout: Layout type required" );
            return;
        } else {
            Widget()->setLayout( qobject_cast< QLayout* >( l->GetQLayout() ) );
        } 
    }
    virtual ~WrappedWidget() {}
protected:
    // problem: not possible to call this method from within constructor method
    // because derived instance is created after base (this) instance and
    // Widget() method might return a pointer to uninitialized data
    // solution: call thid method explicitly from within derived class constructor
    void ConnectSignals() {
        connect( Widget(), SIGNAL( closing() ), this, SIGNAL( closing() ) );
    }
public slots:
    void raise() { Widget()->raise(); }
    void setStyleSheet( const QString& s ) { 
        Widget()->setStyleSheet( s );
    }
    void resize( int w, int h ) { Widget()->resize( w, h ); }
    bool setParentWidget( QObject* obj ) {
        if( !dynamic_cast< WrappedWidget* >( obj ) ) {
            error( "'setParentWidget' requires a WrappedWidget derived object" );
            return false;
        } else {
            SetParentWidget( dynamic_cast< WrappedWidget* >( obj ) );
            return true;
        }
    }
    bool setParentWindow( QObject* obj ) { return setParentWidget( obj ); }
    void close() { Widget()->close(); }
    void setMouseCursor( const QString& shape );
    void setMouseCursor( const QPixmap& pmap, int hotX = -1, int hotY = -1 ) {
        QCursor c( pmap, hotX, hotY );
        Widget()->setCursor( c );
    }
    void setWindowAttribute( const QString& attr ) {
        Widget()->setAttribute( WindowAttribute( attr ) );
    }
    void setWindowIcon( const QPixmap& p ) { Widget()->setWindowIcon( p ); }
    void setWindowIcon( const QString& f ) { Widget()->setWindowIcon( QIcon( f ) ); }
    void move( int x, int y ) { Widget()->move( x, y ); }
    void setWindowStyle( const QStringList& flags );
    void setWindowOpacity( double op ) { Widget()->setWindowOpacity( op ); }
    double windowOpacity() const { return Widget()->windowOpacity(); }
    void setMask( const QPixmap& p ) {
        Widget()->setMask( p.mask() );
    }
    void setMask( const QString& ppath ) {
        QPixmap p( ppath );
        Widget()->setMask( p.mask() );
    }
    void setWindowTitle( const QString& t ) { Widget()->setWindowTitle( t ); }
    QString title() const { return Widget()->windowTitle(); }
    void showNormal() { Widget()->showNormal(); }
    void showFullScreen() { Widget()->showFullScreen(); }
    void show() {  Widget()->show(); }
    void show( int w, int h ) {  Widget()->show(); Widget()->resize( w, h ); }
    void show( int x, int y, int w, int h ) { show( w, h ); move( x, y ); }
    void hide() {  Widget()->hide(); }
signals:
    void closing();
};

}
