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

class Network : public Object {
    Q_OBJECT
public:
    Network() : Object( 0, "LocoNetwork", "/Loco/Network" ) {}
public slots:
    QVariant create( const QString& );
    qint64 tcpSend( int socket, const QByteArray& data ) {
    	tcpSocket_.reset();
    	tcpSocket_.setSocketDescriptor( socket );
    	return tcpSocket_.write( data );
    }
    QByteArray tcpRead( int socket ) {
    	tcpSocket_.reset();
        tcpSocket_.setSocketDescriptor( socket );
        return tcpSocket_.readAll();
    }
private:
    QTcpSocket tcpSocket_;
};

}
