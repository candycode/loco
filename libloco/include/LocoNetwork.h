#pragma once
//#SRCHEADER

#include <QString>
#include <QVariant>
#include <QVariantMap>
#include <QPointer>
#include <QTcpServer>
#include <QTcpSocket>

#include "LocoObject.h"
#include "LocoNetworkAccessManager.h"

///\todo remove
#include <iostream>

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
    void testArrayBuffer( const QByteArray& d ) {
    	float* f = (float*) d.data();
    	std::cout << d.length() << std::endl;
    }
signals:
    void connectionRequest( int socket );
private:
    TcpSocketServer tcpServer_;
};

class TcpSocket : public Object {
	Q_OBJECT
	Q_PROPERTY( int socket READ getSocketDescriptor WRITE setSocketDescriptor )
	Q_PROPERTY( QString errorString READ ErrorString );
	Q_PROPERTY( QString peerName READ PeerName )
	Q_PROPERTY( QString peerAddress READ PeerAddress )
	Q_PROPERTY( quint16 peerPort READ PeerPort )
public:
	TcpSocket() : Object( 0, "LocoTcpSocket", "/Loco/Network/Sockets" ) {
		connect( &socket_, SIGNAL( error( QAbstractSocket::SocketError ) ),
				 this, SLOT( OnSocketError() ) );
	}
	QString ErrorString() const { return socket_.errorString(); }
	QString PeerName() const { return socket_.peerName(); }
	QString PeerAddress() const { return socket_.peerAddress().toString(); }
	quint16 PeerPort() const { return socket_.peerPort(); }
public slots:
    void setSocketDescriptor( int s ) {
    	socket_.setSocketDescriptor( s );
    }
    int getSocketDescriptor() const { return socket_.socketDescriptor(); }
    quint64 send( const QString& data ) {
    	return socket_.write( data.toAscii() );
    }
    quint64 send( const QString& data, int msTimeout ) {
    	quint64 s = send( data );
    	waitForSend( msTimeout );
    }
    bool waitForSend( int msTimeout ) {
    	return socket_.waitForBytesWritten( msTimeout );
    }
    QByteArray recv( int msTimeout ) {
    	socket_.waitForReadyRead( msTimeout );
    	return socket_.readAll();
    }
    bool atEnd() const { return socket_.atEnd(); }
    void close() { socket_.close(); }
    void flush() { socket_.flush(); }
    void connectTo( const QString& host,
    		        quint16 port,
    		        int msTimeout,
    		        const QString& m = "rw"  ) {
    	QIODevice::OpenMode om = 0;
        if( m == "r" ) om = QIODevice::ReadOnly;
    	else if( m == "w" ) om = QIODevice::WriteOnly;
    	else if( m == "rw" || m == "wr" ) om = QIODevice::ReadWrite;
    	else error( "Unknown open mode identifier " + m );
        socket_.connectToHost( host, port, om );
        if( !socket_.waitForConnected( msTimeout ) ) {
        	error( socket_.errorString() );
        }
    }
    void disconnect( int msTimeout ) {
    	socket_.disconnectFromHost();
    	if( !socket_.waitForDisconnected( msTimeout ) ) {
    		error( socket_.errorString() );
    	}
    }
    bool connected() const {
    	return socket_.state() == QAbstractSocket::ConnectedState;
    }
private slots:
    void OnSocketError() {
    	error( socket_.errorString() );
    }
private:
    QTcpSocket socket_;
};


class Network : public Object {
    Q_OBJECT
public:
    Network() : Object( 0, "LocoNetwork", "/Loco/Network" ) {}
public slots:
    QVariant create( const QString& );
};

}
