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
#include <QString>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include "LocoObject.h"


namespace loco {

class Layout : public Object {
    Q_OBJECT
public:
    Layout( Context* c, 
            const QString& n,
            const QString& type, 
            const QString& module,
            bool destroyable,
            ObjectInfo* objInfo ) : Object( c, n, type, module, destroyable, objInfo ) {}
    virtual QObject* GetQLayout() const = 0;
    virtual QString GetLayoutType() const = 0;
public slots:
    void setContentsMargins( int left, int top, int right, int bottom ) {
        qobject_cast< QLayout* >( GetQLayout() )->setContentsMargins( left, top, right, bottom );
    }
    void setMargin( int m ) { 
        qobject_cast< QLayout* >( GetQLayout() )->setMargin( m );
   } 
};

class VBoxLayout : public Layout {
    Q_OBJECT
    Q_PROPERTY( QString layoutType READ GetLayoutType )
public:
    VBoxLayout( Context* c = 0, 
            const QString& n = "LocoVBoxLayout",
            const QString& type = "", 
            const QString& module = "",
            bool destroyable = false,
            ObjectInfo* objInfo = 0 ) : Layout( c, n, type, module, destroyable, objInfo ),
                                        layout_( new QVBoxLayout ) { 
        connect( this, SIGNAL( destroyed() ), layout_, SLOT( deleteLater() ) );
    }
    QString GetLayoutType() const { return "VBOX"; }            
    QObject* GetQLayout() const { return layout_; }
public slots:
    void addWidget( QObject* );
private:
    QVBoxLayout* layout_;
};


//------------------------------------------------------------------------------
class HBoxLayout : public Layout {
    Q_OBJECT
    Q_PROPERTY( QString layoutType READ GetLayoutType )
public:
    HBoxLayout( Context* c = 0, 
            const QString& n = "LocoHBoxLayout",
            const QString& type = "", 
            const QString& module = "",
            bool destroyable = false,
            ObjectInfo* objInfo = 0 ) : Layout( c, n, type, module, destroyable, objInfo ),
                                        layout_( new QHBoxLayout ) {  
        connect( this, SIGNAL( destroyed() ), layout_, SLOT( deleteLater() ) );
    }
    QString GetLayoutType() const { return "HBOX"; }            
    QObject* GetQLayout() const { return layout_; }
public slots:
    void addWidget( QObject* );
private:
    QHBoxLayout* layout_;
};


}
