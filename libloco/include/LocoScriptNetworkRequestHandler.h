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


#include <cstring>

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QString>
#include <QVariant>
#include <QVariantMap>
#include <QMap>
#include <QByteArray>
#include <QTimer>
#include <QUrl>
#include <QList>
#include <QPair>
#include <QVariantList>

#include <LocoObject.h>
#include <LocoINetworkRequestHandler.h>

namespace loco {

QString OpToString( QNetworkAccessManager::Operation op );

class JSNetRequestWrapper : public QObject {
    Q_OBJECT
    Q_PROPERTY( QString url READ url )
    Q_PROPERTY( QString authority READ authority )
    Q_PROPERTY( QString host READ host )
    Q_PROPERTY( int port READ port  )
    Q_PROPERTY( QString path READ path )
    Q_PROPERTY( QString scheme READ scheme )
    Q_PROPERTY( QString operation READ operation )
    Q_PROPERTY( QVariantList headers READ headers )
    Q_PROPERTY( QVariantMap query READ query )
public:
    void Wrap( const QNetworkRequest* nr,
               QNetworkAccessManager::Operation op ) {
        nr_ = nr;
        op_ = OpToString( op );
    }
public slots:
    QString url() const { return nr_->url().toEncoded(); };
    QString authority() const { return nr_->url().authority(); }
    QString host() const { return nr_->url().host(); }
    int port() const { return nr_->url().port(); }
    QString path() const { return nr_->url().path(); }
    QString scheme() const { return nr_->url().scheme(); }
    QString operation() const { return op_; }
    QVariantList headers() const {
        QVariantList headers;
        foreach( QByteArray hname, nr_->rawHeaderList() ) {
            QVariantMap header;
            header["name"] = QString::fromUtf8( hname );
            header["value"] = QString::fromUtf8( nr_->rawHeader( hname ) );
            headers.append( header );
        }
        return headers;
    }
    QVariantMap query() const {
        typedef QList< QPair< QString, QString > > QI;
        QI qi = nr_->url().queryItems();
        QVariantMap m;
        for( QI::const_iterator i = qi.begin(); i != qi.end(); ++i ) {
            if( !m.contains( i->first ) ) m[ i->first ] = QVariantList();
            QVariantList vl =  m[ i->first ].toList();
            vl.push_back( i->second );
            m[ i->first ] = vl;
        }
        return m;
    }
private:
    const QNetworkRequest* nr_;
    QString op_;
};

class NetworkReply : public QNetworkReply {
public:
    NetworkReply() : offset_( 0 ) {}
    void SetContent( const QByteArray& data ) { reset(); content_ = data; }
    void SetHeader( QNetworkRequest::KnownHeaders header, const QVariant& value ) {
        setHeader( header, value );
    }
    void SetUrl( const QUrl& url ) { setUrl( url); }
public: //overridden
    void abort() {}
    qint64 bytesAvailable() const {
        return content_.length();// - offset;
    }
    bool isSequential() const { return true; }
    bool reset() {
        QNetworkReply::reset();
        offset_ = 0;
        return true;
    }
protected:
    qint64 readData( char* data, qint64 maxSize ) {
        if( offset_ < content_.length() ) {
            const qint64 l = std::min( maxSize, content_.length() - offset_ );
            memcpy( data, content_.constData() + offset_, l );
            offset_ += l;
            return l;
        } else return -1;
    }
private:
    qint64 offset_;
    QByteArray content_;
};

class JSNetReplyWrapper : public QObject {
    Q_OBJECT
public:
    void Wrap( NetworkReply* nr ) { nr_ = nr; }
    JSNetReplyWrapper() {
        nameToHeader_[ "ContentType" ] = QNetworkRequest::ContentTypeHeader;
        nameToHeader_[ "ContentLength" ] = QNetworkRequest::ContentLengthHeader;
        nameToHeader_[ "Location" ] = QNetworkRequest::LocationHeader;
        nameToHeader_[ "LastModified" ] = QNetworkRequest::LastModifiedHeader;
        nameToHeader_[ "Cookie" ] = QNetworkRequest::CookieHeader;
        nameToHeader_[ "SetCookie" ] = QNetworkRequest::SetCookieHeader;
    }
public slots:
    void setContent( const QByteArray& data ) { nr_->SetContent( data ); }
    void setHeader( const QString& name, const QVariant& value ) {
        nr_->SetHeader( nameToHeader_[ name ], value );
    }
    void open() {

        nr_->open( QIODevice::ReadOnly | QIODevice::Unbuffered );
    }
    void setUrl( const QString& url ) { nr_->SetUrl( QUrl( url ) ); }
private:
    NetworkReply* nr_;
    QMap< QString, QNetworkRequest::KnownHeaders > nameToHeader_;
};

// Object MUST be the first in the inheritance list if not moc cannot properly resolve derivation!!!
class ScriptNetworkRequestHandler : public Object, public INetworkRequestHandler  {
    Q_OBJECT
public:
    ScriptNetworkRequestHandler() : Object( 0, "LocoScriptNetworkRequestHandler", "Loco/Network/RequestHandler" ) {}
    QNetworkReply* HandleRequest( QNetworkAccessManager::Operation op,
                                  const QNetworkRequest& req,
                                  QIODevice* outgoingData ) {
        req_.Wrap( &req, op );
        NetworkReply* snr = new NetworkReply;
        rep_.Wrap( snr );
        QTimer::singleShot(0, snr, SIGNAL(readyRead()));
        QTimer::singleShot(0, snr, SIGNAL(finished()));
        snr->open( QIODevice::ReadOnly | QIODevice::Unbuffered );
        emit handleRequest( &req_, &rep_ ); //sync emit, returns after slot finishes

        //queue signals
        return snr;
    }
signals:
    void handleRequest( QObject* req, QObject* rep );
private:
    mutable JSNetRequestWrapper req_;
    mutable JSNetReplyWrapper   rep_;
};


}
