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


#include <QGraphicsWidget>
#include <QVariantMap>
#include <QSizeF>
#include <QRectF>

#include "LocoObject.h"
#include "LocoLayout.h"
#include "LocoWindowAttributes.h"
#include "LocoWindowFlags.h"

namespace loco {

///@todo
/// - GraphicsLayout
/// - FrameMargins

//to be able to cast from QObject we need to pass a QObject derived object to qobject_cast 
class  WrappedGraphicsWidget : public Object {
    Q_OBJECT
    Q_PROPERTY( bool autoFillBackground READ GetAutoFillBackground WRITE SetAutoFillBackground )
    Q_PROPERTY( QObject* qGraphicsWidget READ Widget )
    Q_PROPERTY( QVariantMap size READ GetSize WRITE SetSize )
    Q_PROPERTY( qreal opacity READ GetOpacity )
    ///@todo Q_PROPERTY( QVariantMap frameMargins READ GetFrameMargins WRITE SetFrameMargins )
public:
    WrappedGraphicsWidget( Context* c = 0, 
                           const QString& n = "LocoWrappedGraphicsWidget",
                           const QString& type = "", 
                           const QString& module = "",
                           bool destroyable = false,
                           ObjectInfo* objInfo = 0 ) : Object( c, n, type, module, destroyable, objInfo ) {}
    virtual QGraphicsWidget* Widget()  = 0;
    virtual const QGraphicsWidget* Widget() const = 0;
    void SetAutoFillBackground( bool on ) { Widget()->setAutoFillBackground( on ); }
    bool GetAutoFillBackground() const { return Widget()->autoFillBackground(); }
    QVariantMap GetSize() const {
        QVariantMap m;
        const QSizeF sz = Widget()->size();
        m[ "width" ] = sz.width();
        m[ "height" ] = sz.height();
        return m;  
    }
    void SetSize( const QVariantMap& m ) {
        qreal w = 0;
        qreal h = 0;
        if( m.contains( "width"  ) ) w = m[ "width" ].toFloat();
        if( m.contains( "height" ) ) h = m[ "height" ].toFloat();
        if( w > 0 && h > 0 ) Widget()->resize( w, h );
        else error( "Invalid size parameters" );
    }
    qreal GetOpacity() const { return Widget()->opacity(); }
public slots:
    void setWindowAttribute( const QString& attr ) {
        Widget()->setAttribute( WindowAttribute( attr ) );
    }
    void resize( qreal w, qreal h ) { Widget()->resize( w, h ); }
    void setGeometry( const QVariantMap& g ) {
        QRectF geom = Widget()->geometry();
        if( g.contains( "x" ) ) geom.setLeft( g[ "x" ].toFloat() );
        if( g.contains( "y" ) ) geom.setTop( g[ "y" ].toFloat() );
        if( g.contains( "width" ) ) geom.setWidth( g[ "width" ].toFloat() );
        if( g.contains( "height" ) ) geom.setHeight( g[ "height" ].toFloat() ); 
        Widget()->setGeometry( geom );
    }
    void setOpacity( qreal alpha ) { Widget()->setOpacity( alpha ); } 
    void setWindowStyle( const QStringList& flags ) {
        Widget()->setWindowFlags( WindowFlags( flags ) );
    }
};

}
