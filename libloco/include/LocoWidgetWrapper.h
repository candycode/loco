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

#include "LocoWrappedWidget.h"
namespace loco {

//to be able to cast from QObject we need to pass a QObject derived object to qobject_cast 

class  WidgetWrapper : public WrappedWidget {
    Q_OBJECT
    Q_PROPERTY( bool fullScreen READ IsFullScreen )
public:
    WidgetWrapper( QObject* wrappedWidget, //need to use QObject in order to access from Javascript
            Context* c = 0, 
            const QString& n = "LocoWidgetWrapper",
            const QString& type = "", 
            const QString& module = "",
            bool destroyable = false,
            ObjectInfo* objInfo = 0 ) : 
                WrappedWidget( c, n, type, module, destroyable, objInfo ),
                widget_( dynamic_cast< QWidget* >( wrappedWidget ) ) {
                if( widget_ != 0 ) ConnectSignals();

          }
    WidgetWrapper( Context* c = 0, 
            const QString& n = "LocoWrappedWidget",
            const QString& type = "", 
            const QString& module = "",
            bool destroyable = false,
            ObjectInfo* objInfo = 0 ) : 
                WrappedWidget( c, n, type, module, destroyable, objInfo ),
                widget_( 0 ) {}
    bool SetWidget( QObject* ww ) {
        if( !dynamic_cast< QWidget* >( ww ) ) {
            error( "Not a QWidget" );
            return false;
        }
        ///@todo IMPLEMENT
        //if( widget_ != 0 ) DisconnectSignals();
        widget_ = dynamic_cast< QWidget* >( ww );
        connect( widget_, SIGNAL( destroyed() ), this, SIGNAL( destroyed() ) );        
        return true;
    }
public:
    QWidget* Widget() { return widget_; }
    const QWidget* Widget() const { return widget_; }
public slots:
    bool setWidget( QObject* ww ) { return SetWidget( ww ); }
    bool wrap( QObject* ww ) { return SetWidget( ww ); }
    void setLayout( QObject* l ) { SetLayout( l ); }
private:
    QWidget* widget_;
};

}
