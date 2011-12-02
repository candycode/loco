//#SRCHEADER

#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QBuffer>

#include "LocoNetwork.h"
#include "LocoContext.h"
#include "LocoURLUtility.h"

namespace loco {


QVariantMap Http::get( const QString& urlString, int timeout, const QVariantMap& opt ) {
	QEventLoop loop;
	QVariantMap reply;
	QUrl url = ConfigureURL( urlString, opt );
	QObject::connect( nam_, SIGNAL( finished( QNetworkReply*) ), &loop, SLOT( quit() ) );
	// soft real-time guarantee: kill network request if the total time is >= timeout
	QTimer::singleShot( timeout, &loop, SLOT( quit() ) );
	QNetworkRequest request;
    request.setUrl( url );
    if( opt.contains( "headers" ) ) ConfigureHeaders( request, opt[ "headers" ].toMap() );
    QNetworkReply* nr = nam_->get( request );
	// Execute the event loop here, now we will wait here until readyRead() signal is emitted
	// which in turn will trigger event loop quit.
	loop.exec();
	if( nr->error() != QNetworkReply::NoError ) {
		error( nr->errorString() );
        return reply;
	}
	if( !nr->isFinished() ) {
		error( "Network request timeout" );
		return reply;
	}
    reply[ "content" ] = nr->readAll();
    QVariantMap headers;
    typedef QList< QByteArray > HEADERS;
    HEADERS rawHeaders = nr->rawHeaderList();
    for( HEADERS::const_iterator i = rawHeaders.begin(); i != rawHeaders.end(); ++i ) {
    	if( nr->hasRawHeader( *i ) ) {
    		headers[ QString( *i ) ] = QString( nr->rawHeader( *i ) );
    	}
    }
    reply[ "headers" ] = headers;
    return reply;
}

QVariantMap Http::head( const QString& urlString, int timeout, const QVariantMap& opt ) {
	QEventLoop loop;
	QVariantMap reply;
	QUrl url = ConfigureURL( urlString, opt );
	QObject::connect( nam_, SIGNAL( finished( QNetworkReply*) ), &loop, SLOT( quit() ) );
	// soft real-time guarantee: kill network request if the total time is >= timeout
	QTimer::singleShot( timeout, &loop, SLOT( quit() ) );
	QNetworkRequest request;
    request.setUrl( url );
    if( opt.contains( "headers" ) ) ConfigureHeaders( request, opt[ "headers" ].toMap() );
    QNetworkReply* nr = nam_->head( request );
	// Execute the event loop here, now we will wait here until readyRead() signal is emitted
	// which in turn will trigger event loop quit.
	loop.exec();
	if( nr->error() != QNetworkReply::NoError ) {
		error( nr->errorString() );
        return reply;
	}
	if( !nr->isFinished() ) {
		error( "Network request timeout" );
		return reply;
	}
    QVariantMap headers;
    typedef QList< QByteArray > HEADERS;
    HEADERS rawHeaders = nr->rawHeaderList();
    for( HEADERS::const_iterator i = rawHeaders.begin(); i != rawHeaders.end(); ++i ) {
    	if( nr->hasRawHeader( *i ) ) {
    		headers[ QString( *i ) ] = QString( nr->rawHeader( *i ) );
    	}
    }
    reply[ "headers" ] = headers;
    return reply;
}


QVariantMap Http::request( const QString& urlString,
	                       const QByteArray& verb,
    		               int timeout,
	                       const QVariantMap& opt,
	                       const QByteArray& data ) {
	QEventLoop loop;
	QVariantMap reply;
	QUrl url = ConfigureURL( urlString, opt );
	QObject::connect( nam_, SIGNAL( finished( QNetworkReply*) ), &loop, SLOT( quit() ) );
	// soft real-time guarantee: kill network request if the total time is >= timeout
	QTimer::singleShot( timeout, &loop, SLOT( quit() ) );
	QNetworkRequest request;
	request.setUrl( url );
	if( opt.contains( "headers" ) ) ConfigureHeaders( request, opt[ "headers" ].toMap() );
	QBuffer datain;
	datain.setData( data );
	QNetworkReply* nr = nam_->sendCustomRequest( request, verb, &datain );
	// Execute the event loop here, now we will wait here until readyRead() signal is emitted
	// which in turn will trigger event loop quit.
	loop.exec();
	if( nr->error() != QNetworkReply::NoError ) {
		error( nr->errorString() );
		return reply;
	}
	if( !nr->isFinished() ) {
		error( "Network request timeout" );
		return reply;
	}
	reply[ "content" ] = nr->readAll();
	QVariantMap headers;
	typedef QList< QByteArray > HEADERS;
	HEADERS rawHeaders = nr->rawHeaderList();
	for( HEADERS::const_iterator i = rawHeaders.begin(); i != rawHeaders.end(); ++i ) {
		if( nr->hasRawHeader( *i ) ) {
			headers[ QString( *i ) ] = QString( nr->rawHeader( *i ) );
		}
	}
	reply[ "headers" ] = headers;
	return reply;
}


QVariantMap Http::post( const QString& urlString,
		                int timeout,
		                const QByteArray& data,
		                const QVariantMap& opt ) {
	QEventLoop loop;
	QVariantMap reply;
	QUrl url = ConfigureURL( urlString, opt );
	QObject::connect( nam_, SIGNAL( finished( QNetworkReply*) ), &loop, SLOT( quit() ) );
	// soft real-time guarantee: kill network request if the total time is >= timeout
	QTimer::singleShot( timeout, &loop, SLOT( quit() ) );
	QNetworkRequest request;
    request.setUrl( url );
    if( opt.contains( "headers" ) ) ConfigureHeaders( request, opt[ "headers" ].toMap() );
    QNetworkReply* nr = nam_->post( request, data );
	// Execute the event loop here, now we will wait here until readyRead() signal is emitted
	// which in turn will trigger event loop quit.
	loop.exec();
	if( nr->error() != QNetworkReply::NoError ) {
		error( nr->errorString() );
        return reply;
	}
	if( !nr->isFinished() ) {
		error( "Network request timeout" );
		return reply;
	}
    reply[ "content" ] = nr->readAll();
    QVariantMap headers;
    typedef QList< QByteArray > HEADERS;
    HEADERS rawHeaders = nr->rawHeaderList();
    for( HEADERS::const_iterator i = rawHeaders.begin(); i != rawHeaders.end(); ++i ) {
    	if( nr->hasRawHeader( *i ) ) {
    		headers[ QString( *i ) ] = QString( nr->rawHeader( *i ) );
    	}
    }
    reply[ "headers" ] = headers;
    return reply;
}


QVariant Network::create( const QString& name ) {
	if( name == "Http" ) {
		if( !GetContext()->GetNetworkAccessManager() ) throw std::runtime_error( "NULL Network Access Manager" );
        Http* http = new Http( qobject_cast< NetworkAccessManager* >( GetContext()->GetNetworkAccessManager() ) );
		return GetContext()->AddObjToJSContext( http );
	} else if( name == "tcp-server" ) {
       TcpServer* tcp = new TcpServer;
       return GetContext()->AddObjToJSContext( tcp );
	} else if( name == "tcp-socket" ) {
	   TcpSocket* tcp = new TcpSocket;
	   return GetContext()->AddObjToJSContext( tcp, false );
	} else if( name == "udp-socket" ) {
	   UdpSocket* udp = new UdpSocket;
	   return GetContext()->AddObjToJSContext( udp, false );
	} else if( name == "tcp-ssl-socket" ) {
	   SslSocket* ssl = new SslSocket;
	   return GetContext()->AddObjToJSContext( ssl, false );
	} else {
		error( "Unknown type " + name );
		return QVariant();
	}
}

}

/*
 * #include <QApplication>
#include <QSslSocket>
#include <QDebug>

int main( int argc, char **argv )
{
    QApplication app( argc, argv );

    QSslSocket socket;
    socket.connectToHostEncrypted( "bugs.kde.org", 443 );

    if ( !socket.waitForEncrypted() ) {
        qDebug() << socket.errorString();
        return 1;
    }

    socket.write( "GET / HTTP/1.1\r\n" \
                  "Host: bugs.kde.org\r\n" \
                  "Connection: Close\r\n\r\n" );
    while ( socket.waitForReadyRead() ) {
        qDebug() <<  socket.readAll().data();
    };

    qDebug() << "Done";

    return 0;
}
 */

/*
 * #include "Socket.h"
#include "Socket.h"

Socket::Socket( QObject *parent )
:
	QSslSocket( parent ),
{
	setLocalCertificate( ":ssl.pem" );
	setPrivateKey( ":ssl.pem" );
	connectToHostEncrypted( m_host, m_port );
}
#ifndef SOCKET_H
#ifndef SOCKET_H
#define SOCKET_H

#include <QSslSocket>

class Socket: public QSslSocket
{
	Q_OBJECT

public:
	Socket( QObject *parent = 0 );
};

#endif
#include "server.h"
#include "server.h"

#include "server_client.h"

Server::Server( QObject *parent )
	:QTcpServer( parent )
{
	listen( QHostAddress::Any, 12345 );
}

void Server::incomingConnection( int socketDescriptor )
{
	new Client( socketDescriptor, this );
}
#ifndef SERVER_H
#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>

class Server : public QTcpServer
{
	Q_OBJECT

public:
	Server( QObject *parent = 0 );

protected:
	void incomingConnection( int socketDescriptor );
};

#endif

#include "server_client.h"
#include "server_client.h"

Client::Client( int socketDescriptor, QObject *parent )
	: QSslSocket( parent )
{
	connect( this, SIGNAL(disconnected()), SLOT(deleteLater()) );
	connect( this, SIGNAL(sslErrors(QList<QSslError>)),
			SLOT(sslErrors(QList<QSslError>)) );

	if( !setSocketDescriptor( socketDescriptor ) )
	{
		deleteLater();
		return;
	}

	setLocalCertificate( "ssl.pem" );
	setPrivateKey( "ssl.pem" );
	startServerEncryption();
}

void Client::sslErrors( const QList<QSslError> &errors )
{
	foreach( const QSslError &error, errors )
	{
		switch( error.error() )
		{
		case QSslError::NoPeerCertificate: ignoreSslErrors(); break;
		default:
			qWarning( "CLIENT SSL: error %s", qPrintable(error.errorString()) );
			disconnect(); return;
		}
	}
}
#ifndef CLIENT_H
#ifndef CLIENT_H
#define CLIENT_H

#include <QSslSocket>

class Client: public QSslSocket
{
	Q_OBJECT

public:
	Client( int socketDescriptor, QObject *parent );

private slots:
	void sslErrors( const QList<QSslError> &err );
};

#endif
 */




