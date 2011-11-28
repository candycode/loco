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
	} else {
		error( "Unknown type " + name );
		return QVariant();
	}
}

}
