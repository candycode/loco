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


#include <QString>
#include <QVariant>
#include <QVariantMap>
#include <QPointer>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QSslSocket>
#include <QStringList>
#include <QList>
#include <QSslError>
#include <QSslKey>
#include <QSslCertificate>

#include "LocoObject.h"
#include "LocoNetworkAccessManager.h"

namespace loco {

class Http : public Object {
    Q_OBJECT
private:
    typedef QVariantMap (Http::*Request)(const QString& , const QVariant& , int );
public:
    Http( NetworkAccessManager* nam ) :
        Object( 0, "LocoHttp", "/Loco/Network/Http" ), nam_( nam ) {}
public slots:
    QVariantMap get ( const QString& urlString, int timeout, const QVariantMap& opt = QVariantMap() );
    QVariantMap post( const QString& urlString,
                      int timeout,
                      const QByteArray& data,
                      const QVariantMap& opt = QVariantMap() );
    QVariantMap head( const QString& urlString, int timeout, const QVariantMap& opt = QVariantMap() );
    QVariantMap request( const QString& urlString,
                         const QByteArray& verb,
                         int timeout,
                         const QVariantMap& opt = QVariantMap(),
                         const QByteArray& data = QByteArray() );
private:
    QPointer< NetworkAccessManager > nam_;
};

class TcpSocketServer : public QTcpServer {
    Q_OBJECT
protected:
    void incomingConnection( int socket ) {
        emit connectionRequest( socket );
    }
signals:
    void connectionRequest( int );
};

class TcpServer : public Object {
    Q_OBJECT
public:
    TcpServer() : Object( 0, "LocoTcpServer", "/Loco/Network/Sockets" ) {
        connect( &tcpServer_, SIGNAL( connectionRequest( int ) ), this, SIGNAL( connectionRequest( int ) ) );
    }
public slots:
    bool listen( quint16 port, const QString& address = "any" ) {
        ///\todo use 'address' parameter
        return tcpServer_.listen( QHostAddress::Any, port );
    }
signals:
    void connectionRequest( int socket );
private:
    TcpSocketServer tcpServer_;
};

class Socket : public Object {
    Q_OBJECT
    Q_PROPERTY( int socket READ getSocketDescriptor WRITE setSocketDescriptor )
    Q_PROPERTY( QString errorMessage READ ErrorString );
    Q_PROPERTY( QString peerName READ PeerName )
    Q_PROPERTY( QString peerAddress READ PeerAddress )
    Q_PROPERTY( quint16 peerPort READ PeerPort )
    Q_PROPERTY( QString localAddress READ LocalAddress )
    Q_PROPERTY( quint16 localPort READ LocalPort )
public:
    Socket( const QString& n )
    : Object( 0, n, "/Loco/Network/Socket" )  {}
    QString ErrorString() const { return socket_->errorString(); }
    QString PeerName() const { return socket_->peerName(); }
    QString PeerAddress() const { return socket_->peerAddress().toString(); }
    quint16 PeerPort() const { return socket_->peerPort(); }
    QString LocalAddress() const { return socket_->localAddress().toString(); }
    quint16 LocalPort() const { return socket_->localPort(); }
public slots:
    QByteArray read( qint64 maxSize ) { return socket_->read( maxSize ); }
    void write( const QString& data ) { socket_->write( data.toAscii(), data.toAscii().length() ); }
    quint64 bytesAvailable() const { return socket_->bytesAvailable(); }
    quint64 bytesToWrite() const { return socket_->bytesToWrite(); }
    bool isOpen() const { return socket_->isOpen(); }
    ///\todo remove
    bool waitForRecv( int msTimeout ) {
        return socket_->waitForReadyRead( msTimeout );
    }
    bool waitForReadyRead( int msTimeout ) {
        return socket_->waitForReadyRead( msTimeout );
    }
    bool setSocketDescriptor( int s ) {
        return socket_->setSocketDescriptor( s );
    }
    int getSocketDescriptor() const { return socket_->socketDescriptor(); }
    quint64 send( const QString& data ) {
        return socket_->write( data.toAscii() );
    }
    quint64 send( const QString& data, int msTimeout ) {
        quint64 s = send( data );
        waitForSend( msTimeout );
        return s;
    }
    bool waitForSend( int msTimeout ) {
        return socket_->waitForBytesWritten( msTimeout );
    }
    bool waitForBytesWritten( int msTimeout ) {
        return socket_->waitForBytesWritten( msTimeout );
    }
    QString recv( int msTimeout ) {
        socket_->waitForReadyRead( msTimeout );
        return socket_->readAll();
    }
    QString readAll() const { return socket_->readAll(); }
    bool atEnd() const { return socket_->atEnd(); }
    void close() { socket_->close(); }
    void flush() { socket_->flush(); }
    QString errorMsg() const { return socket_->errorString(); }
private slots:
    void OnSocketError() {
        Object::error( socket_->errorString() );
    }
signals:
    void error();
    void bytesWritten( qint64 );
    void readyRead();
protected:
    void SetSocket( QAbstractSocket* s ) {
        socket_ = s;
        connect( socket_, SIGNAL( error( QAbstractSocket::SocketError ) ),
                 this, SLOT( OnSocketError() ) );
        connect( socket_, SIGNAL( bytesWritten( qint64 ) ), this, SIGNAL( bytesWritten( qint64 ) ) );
        connect( socket_, SIGNAL( readyRead() ), this, SIGNAL( readyRead() ) );
    }
private:
    QAbstractSocket* socket_;
};

class TcpSocket : public Socket {
    Q_OBJECT
public:
    TcpSocket() : Socket( "LocoTcpSocket" ), socket_( new QTcpSocket ) {
        socket_->setParent( this );
        Socket::SetSocket( socket_ );
        connect( socket_, SIGNAL( connected() ), this, SIGNAL( connected() ) );
        connect( socket_, SIGNAL( disconnected() ), this, SIGNAL( disconnected() ) );
    }
public slots:
    void connectTo( const QString& host,
                    quint16 port,
                    int msTimeout,
                    const QString& m = "rw"  ) {
        QIODevice::OpenMode om = 0;
        if( m == "r" ) om = QIODevice::ReadOnly;
        else if( m == "w" ) om = QIODevice::WriteOnly;
        else if( m == "rw" || m == "wr" ) om = QIODevice::ReadWrite;
        else Object::error( "Unknown open mode identifier " + m );
        socket_->connectToHost( host, port, om );
        if( !socket_->waitForConnected( msTimeout ) ) {
            Object::error( socket_->errorString() );
        }
    }
    void disconnect( int msTimeout ) {
        socket_->disconnectFromHost();
        if( !socket_->waitForDisconnected( msTimeout ) ) {
            Object::error( socket_->errorString() );
        }
    }
    bool isConnected() const {
        return socket_->state() == QAbstractSocket::ConnectedState;
    }
signals:
    void connected();
    void disconnected();
private:
    QTcpSocket* socket_;
};

class UdpSocket : public Socket {
    Q_OBJECT
public:
    UdpSocket() : Socket( "LocoUdpSocket" ), socket_( new QUdpSocket ) {
        socket_->setParent( this );
        Socket::SetSocket( socket_ );
    }
public slots:
    bool bind( const QString& host, quint16 port, const QStringList& mode = QStringList() ) {
        QUdpSocket::BindMode bm = 0;
        if( mode.isEmpty() ) bm = QUdpSocket::DefaultForPlatform;
        else {
            for( QStringList::const_iterator i = mode.begin();
                i != mode.end(); ++i ) {
                if( *i == "share" ) bm |= QUdpSocket::ShareAddress;
                else if( *i == "reuse ") bm |= QUdpSocket::ReuseAddressHint;
                else if( *i == "don't share" ) bm |= QUdpSocket::DontShareAddress;
                else if( *i == "default" ) bm |= QUdpSocket::DefaultForPlatform;
            }
        }
        return socket_->bind( QHostAddress( host ), port, bm );
    }
    bool hasData() const { return socket_->hasPendingDatagrams(); }
    qint64 dataSize() const { return socket_->pendingDatagramSize(); }
    qint64 sendTo( const QString& data, const QString& host, quint16 port ) {
        QByteArray ba = data.toAscii();
        QHostAddress h( host );
        qint64 sent = socket_->writeDatagram( ba.data(), ba.size(), h, port );
        while( sent < ba.size() && sent > 0 ) {
            sent += socket_->writeDatagram( ba.data() + sent, ba.size() - sent, h, port );
        }
        return sent;
    }
    QString recvFrom( const QString& host, quint16 port ) {
        QByteArray dout;
        QHostAddress sender( host );
        quint16 senderPort = port;
        while( socket_->hasPendingDatagrams() ) {
            QByteArray datagram;
            datagram.resize( socket_->pendingDatagramSize() );
             socket_->readDatagram( datagram.data(), datagram.size(),
                                   &sender, &senderPort);
             dout.append( datagram );
        }
        return dout;
    }
private:
    QUdpSocket* socket_;
};

class SslSocket : public Socket {
    Q_OBJECT
    Q_PROPERTY( QStringList sslErrors READ SSLErrors )
public:
    SslSocket() : Socket( "LocoSslSocket" ), socket_( new QSslSocket ) {
        socket_->setParent( this );
        Socket::SetSocket( socket_ );
        connect( socket_, SIGNAL( connected() ), this, SIGNAL( connected() ) );
        connect( socket_, SIGNAL( disconnected() ), this, SIGNAL( disconnected() ) );
        connect( socket_, SIGNAL( encrypted() ), this, SIGNAL( encrypted() ) );
        connect( socket_, SIGNAL( encryptedBytesWritten( qint64 ) ), this, SIGNAL( encryptedBytesWritten( qint64 ) ) );
    }
    QStringList SSLErrors() const {
        QStringList el;
        const QList< QSslError > errors = socket_->sslErrors();
        for( QList< QSslError >::const_iterator i = errors.begin();
             i != errors.end(); ++i    ) {
            el << i->errorString();
        }
        return el;
    }
public slots:
    void setLocalCertificate( const QString& cert ) {
        socket_->setLocalCertificate( QSslCertificate( cert.toAscii() ) );
    }
    void setLocalCertificateFromFile( const QString& certFile ) {
           socket_->setLocalCertificate( certFile );
    }
    void setPrivateKey( const QString& key, const QString& algo ) {
        if( algo.toLower() == "rsa" ) {
            socket_->setPrivateKey( key.toAscii(), QSsl::Rsa );
        } else if( algo.toLower() == "dsa" ) {
            socket_->setPrivateKey( key.toAscii(), QSsl::Dsa );
        } else {
            Object::error( "Unknown algorithm -- " + algo );
        }
    }
    void setPrivateKeyFromFile( const QString& filePath, const QString& algo ) {\
        if( algo.toLower() == "rsa" ) {
            socket_->setPrivateKey( filePath, QSsl::Rsa );
        } else if( algo.toLower() == "dsa" ) {
            socket_->setPrivateKey( filePath, QSsl::Dsa );
        } else {
            Object::error( "Unknown algorithm -- " + algo );
        }

    }
    QString getPrivateKey() const { return socket_->privateKey().toPem().constData(); }
    QString getLocalCertificate() const { return socket_->localCertificate().toPem().constData(); }
    void startServerEncryption( int msTimeout ) {
        socket_->startServerEncryption();
        socket_->waitForEncrypted( msTimeout );
    }
    void connectTo( const QString& host,
                    quint16 port,
                    int msTimeout = -1, //non blocking by default
                    const QString& m = "rw",
                    const QString& sslPeerName = QString() ) { //use a different host name for cert. validation)
        QIODevice::OpenMode om = 0;
        if( m == "r" ) om = QIODevice::ReadOnly;
        else if( m == "w" ) om = QIODevice::WriteOnly;
        else if( m == "rw" || m == "wr" ) om = QIODevice::ReadWrite;
        else Object::error( "Unknown open mode identifier " + m );
        if( !sslPeerName.isEmpty() ) socket_->connectToHostEncrypted( host, port, sslPeerName, om );
        else socket_->connectToHostEncrypted( host, port, QIODevice::ReadWrite );
        if( msTimeout > 0 && !socket_->waitForEncrypted( msTimeout ) ) {
            Object::error( socket_->errorString() );
        }
    }
    void ignoreSSLErrors() { socket_->ignoreSslErrors(); }
    void disconnect( int msTimeout ) {
        socket_->disconnectFromHost();
        if( !socket_->waitForDisconnected( msTimeout ) ) {
            Object::error( socket_->errorString() );
        }
    }
    bool isEncrypted() const { return socket_->isEncrypted(); }
    bool isConnected() const {
        return socket_->state() == QAbstractSocket::ConnectedState;
    }
    bool waitForEncrypted( int msTimeout ) {
        return socket_->waitForEncrypted( msTimeout );
    }
    void setPeerVerifyMode( const QString& mode ) {
        QSslSocket::PeerVerifyMode vm;
        if( mode == "none" ) vm = QSslSocket::VerifyNone;
        else if( mode == "query-peer" ) vm = QSslSocket::QueryPeer;
        else if( mode == "verify-peer" ) vm = QSslSocket::VerifyPeer;
        else if( mode == "auto" ) vm = QSslSocket::AutoVerifyPeer;
        else {
            Object::error( "Unknown peer verify mode -- " + mode );
            return;
        }
        socket_->setPeerVerifyMode( vm );
    }
signals:
    void connected();
    void disconnected();
    void encrypted();
    void encryptedBytesWritten ( qint64 );
    //void modeChanged ( QSslSocket::SslMode mode )
    //void peerVerifyError ( const QSslError & error )
    //void sslErrors ( const QList<QSslError> & errors )
private:
    QSslSocket* socket_;
};


class Network : public Object {
    Q_OBJECT
public:
    Network() : Object( 0, "LocoNetwork", "/Loco/Network" ) {}
public:
    Network* Clone() const { return new Network(); }
public slots:
    QVariant create( const QString& );
    bool cloneable() const { return true; }
    bool ssl() const { return QSslSocket::supportsSsl(); }
};

}
