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

#include <QMap>
#include <QImage>
#include <QPixmap>
#include <QVariantMap>
#include <QString>

#include "LocoObject.h"

namespace loco {

class Context;

class Image2D : public Object {
    Q_OBJECT
    Q_PROPERTY( bool empty READ Empty )
    Q_PROPERTY( int width READ GetWidth )
    Q_PROPERTY( int height READ GetHeight )
    Q_PROPERTY( QString format READ GetFormat WRITE SetFormat )
    Q_PROPERTY( int depth READ GetDepth )
    Q_PROPERTY( QPixmap pixmap READ GetPixmap WRITE SetPixmap )
    Q_PROPERTY( QImage image READ GetImage WRITE SetImage )
    Q_PROPERTY( QStringList keys READ GetKeys )
public:
   Image2D( Context* c = 0 ) : Object( c, "LocoImage2D", "Loco/Graphics/Image" ) {}
   Image2D( const QImage& i, Context* c ) : Object( c, "LocoImage2D", "Loco/Graphics/Image" ),
                                            image_( i ) {} 
   bool Empty() const { return image_.isNull(); }
   int GetWidth() const { return image_.width(); }
   int GetHeight() const { return image_.height(); }
   QString GetFormat() const { return MapFormat( image_.format() ); }
   void SetFormat( const QString& format ) { image_ = image_.convertToFormat( MapFormat( format ) ); }
   int GetDepth() const { return image_.depth(); }
   QPixmap GetPixmap() const { QPixmap p; p.convertFromImage( image_ ); return p; }
   const QImage& GetImage() const { return image_; }
   void Swap( QImage& i ) { image_.swap( i ); }
   QStringList GetKeys() const { return image_.textKeys(); }
   void SetPixmap( const QPixmap& p ) {  image_ = p.toImage(); }
   void SetImage( const QImage& i ) { image_ = i; }
public slots:
    void create( quint64 address, int width, int height, const QString& format ) {
        image_ = QImage( reinterpret_cast< unsigned char* >( address ), width, height, MapFormat( format ) );
    }
    void create( int width, int height, const QString& format ) {
        image_ = QImage( width, height, MapFormat( format ) );
    }
    bool load( const QString& filename, const QString& format = QString() ) {
        image_ = format.isEmpty() ? QImage( filename ) : 
                 QImage( filename, format.toAscii().constData() );
        if( Empty() ) error( "Cannot load image from file " + filename );
        return !Empty(); 
    }
    void create( const QPixmap& p ) { image_ = p.toImage(); }
    bool save( const QString& filename, const QString& format = QString(), int quality = -1 ) {
        if( format.isEmpty() ) return image_.save( filename, 0, quality );
        else return image_.save( filename, format.toAscii().constData(), quality );  
    }
    void resize( int width, int height ) {
        if( width == GetWidth() && height == GetHeight() ) return;
        image_ = image_.scaled( width, height );
    }
    QObject* copy( int x = 0, int y = 0, int w = -1, int h = -1 ) {
        if( w == -1 ) w = GetWidth();
        if( h == -1 ) h = GetHeight();
        QImage i = image_.copy( x, y, w, h );
        return new Image2D( i, GetContext() );
    }
    void scaleToHeight( int h ) {
        image_ = image_.scaledToHeight( h );
    }
    void scaleToWidth( int w ) {
        image_ = image_.scaledToWidth( w );
    }
    void swap( QObject* oth ) {
        if( qobject_cast< Image2D* >( oth ) == 0 ) {
            error( "Not an image object" );
        } else {
            Image2D* i = qobject_cast< Image2D* >( oth );
            i->Swap( image_ ); 
        } 
    }
    QString text( const QString& key ) const { return image_.text( key ); }
private:
    QImage::Format MapFormat( const QString& f ) const {
        static QMap< QString, QImage::Format > fmap;
        if( fmap.isEmpty() ) {
          fmap[ "invalid" ] = QImage::Format_Invalid;
          fmap[ "mono"    ] = QImage::Format_Mono;	
          fmap[ "monoLSB" ] = QImage::Format_MonoLSB;
          fmap[ "index8"  ] = QImage::Format_Indexed8;
          fmap[ "rgb32"   ] = QImage::Format_RGB32;
          fmap[ "argb32"  ] = QImage::Format_ARGB32;
          fmap[ "argb32-premul" ] = QImage::Format_ARGB32_Premultiplied;
          fmap[ "rgb16" ] = QImage::Format_RGB16;
          fmap[ "argb8565-premul" ] = QImage::Format_ARGB8565_Premultiplied;
          fmap[ "rgb666" ] = QImage::Format_RGB666;
          fmap[ "argb666-premul" ] = QImage::Format_ARGB6666_Premultiplied;
          fmap[ "rgb555" ] = QImage::Format_RGB555;
          fmap[ "argb8555-premul" ] = QImage::Format_ARGB8555_Premultiplied;
          fmap[ "rgb888" ] = QImage::Format_RGB888;
          fmap[ "rgb444" ] = QImage::Format_RGB444;
          fmap[ "argb4444-premul" ] = QImage::Format_ARGB4444_Premultiplied;
        }
        return fmap[ f ];
    }
    const QString& MapFormat( QImage::Format f ) const {
        static QMap< QImage::Format, QString > fmap;
        if( fmap.isEmpty() ) {
          fmap[ QImage::Format_Invalid ] = "invalid";
          fmap[ QImage::Format_Mono    ] = "mono"; 
          fmap[ QImage::Format_MonoLSB ] = "monoLSB";
          fmap[ QImage::Format_Indexed8 ] = "index8";
          fmap[ QImage::Format_RGB32 ] = "rgb32";
          fmap[ QImage::Format_ARGB32 ] = "argb32";
          fmap[ QImage::Format_ARGB32_Premultiplied ] = "argb32-premul";
          fmap[ QImage::Format_RGB16 ] = "rgb16";
          fmap[ QImage::Format_ARGB8565_Premultiplied ] = "argb8565-premul";
          fmap[ QImage::Format_RGB666 ] = "rgb666";
          fmap[ QImage::Format_ARGB6666_Premultiplied ] = "argb666-premul";
          fmap[ QImage::Format_RGB555 ] = "rgb555";
          fmap[ QImage::Format_ARGB8555_Premultiplied ] = "argb8555-premul";
          fmap[ QImage::Format_RGB888 ] = "rgb888";
          fmap[ QImage::Format_RGB444 ] = "rgb444";
          fmap[ QImage::Format_ARGB4444_Premultiplied ] = "argb4444-premul";
        }
        return fmap[ f ];
    } 
private:
    QImage image_;
};

}
