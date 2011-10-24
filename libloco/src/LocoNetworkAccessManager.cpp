//#SRCHEADER

#include "LocoNetworkAccessManager.h"

#include <QDate>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QSettings>
#include <QNetworkDiskCache>
#include <QDesktopServices>
#include <QSslConfiguration>

#include <stdexcept>

///@todo remove
#include <iostream>

namespace loco {


namespace {
QString OpToString( QNetworkAccessManager::Operation op ) {
    switch ( op ) {
    case QNetworkAccessManager::GetOperation:
        return "GET";
        break;
    case QNetworkAccessManager::PutOperation:
        return "PUT";
        break;
    case QNetworkAccessManager::PostOperation:
        return "POST";
        break;
    case QNetworkAccessManager::HeadOperation:
        return "HEAD";
        break;
    case QNetworkAccessManager::DeleteOperation:
        return "DELETE";
        break;
    default:
        return "";
        break;
    }
    return "";
}
}




class NetworkCookieJar : public QNetworkCookieJar {
public:
	NetworkCookieJar( QObject* parent = 0 ) : QNetworkCookieJar( parent ), cookieFile_( "loco-cookies.ini" ) {}
	void SetCookieFile( const QString& cf ) { cookieFile_ = cf; }
	const QString& CookieFile() const { return cookieFile_; }
    bool setCookiesFromUrl ( const QList<QNetworkCookie>& cookieList, const QUrl & url ) {
        //std::cout << "setCookiesFromUrl: " << url.host().toStdString() << "\n";
	    QSettings settings(cookieFile_, QSettings::IniFormat);
        settings.beginGroup(url.host());      
        for ( QList<QNetworkCookie>::const_iterator i = cookieList.begin() ; i != cookieList.end() ; i++ ) {
            settings.setValue((*i).name(), QString((*i).value()));
            //std::cout << "  [*] Saving cookie: " << QString((*i).name()).toStdString() << "\n";
        }
        settings.sync();
        return true;
   } 
   QList<QNetworkCookie> cookiesForUrl ( const QUrl & url ) const {
       //std::cout << "cookiesFromUrl: " << url.host().toStdString() << "\n";
       QSettings settings(cookieFile_, QSettings::IniFormat);
       QList<QNetworkCookie> cookieList;
       settings.beginGroup(url.host());
       QStringList keys = settings.childKeys();
       for ( QStringList::iterator i = keys.begin() ; i != keys.end() ; i++ ) {
           //std::cout << "  Loading cookie: " << QString((*i)).toStdString() << "\n";
           cookieList.push_back(QNetworkCookie((*i).toLocal8Bit(), settings.value(*i).toByteArray()));
       }
       return cookieList;
   }
private:
	QString cookieFile_;
};

NetworkAccessManager::NetworkAccessManager( QObject* p ) 
    : QNetworkAccessManager( p ), 
      rxPattern_( QRegExp::RegExp2 ),
      filterRequests_( true ),
      allowNetAccess_( false ),
      enableUrlMapping_( false ),
      signalAccessDenied_( true ),
      logRequests_( false ),
      emitRequestSignal_( false ),
      ignoreSSLErrors_( true ) {
        setCookieJar( new NetworkCookieJar );  
        networkDiskCache_ = new QNetworkDiskCache( this );
        //networkDiskCache_->setCacheDirectory( "./" );//
		networkDiskCache_->setCacheDirectory(QDesktopServices::storageLocation(QDesktopServices::CacheLocation));
		setCache( networkDiskCache_ );
		connect( this, SIGNAL( authenticationRequired( QNetworkReply*, QAuthenticator* ) ),
			     SLOT( OnAuthenticateRequest( QNetworkReply*,QAuthenticator* ) ) );
		
}

QNetworkReply* NetworkAccessManager::createRequest( Operation op,
                                                    const QNetworkRequest& req,
                                                    QIODevice* outgoingData  ) {
	if( logRequests_ || emitRequestSignal_ ) {
		typedef QList< QPair< QString, QString > > QI;
		QI qi = req.url().queryItems();
		QVariantMap requestLog;
		requestLog[ "url"       ] = QString( req.url().toEncoded() );
		requestLog[ "authority" ] = req.url().authority();
		requestLog[ "host"      ] = req.url().host();
		requestLog[ "port"      ] = req.url().port();
		requestLog[ "path"      ] = req.url().path();
		requestLog[ "scheme"    ] = req.url().scheme();
		requestLog[ "username"  ] = req.url().userName();
		requestLog[ "password"  ] = req.url().password();
		requestLog[ "date"      ] = QDate::currentDate();
		requestLog[ "operation" ] = OpToString( op );
		QVariantList headers;
		foreach( QByteArray hname, req.rawHeaderList() ) {
            QVariantMap header;
            header["name"] = QString::fromUtf8( hname );
            header["value"] = QString::fromUtf8( req.rawHeader( hname ) );
			headers.append( header );
		}
		requestLog[ "headers" ] = headers;
   		QVariantMap m;
		for( QI::const_iterator i = qi.begin(); i != qi.end(); ++i ) {
			m[ i->first ]  = i->second;
		}
		requestLog[ "query" ] = m;
		if( outgoingData != 0 ) {
			requestLog[ "data" ] = outgoingData->peek( outgoingData->bytesAvailable() );
		}
		if( logRequests_ ) requests_.push_back( requestLog );
		if( emitRequestSignal_ ) emit OnRequest( requestLog );
	}
    if( !allowNetAccess_ ) {
        emit UnauthorizedNetworkAccessAttempt();
		QNetworkRequest nr;
	    nr.setUrl( defaultUrl_ );
        return QNetworkAccessManager::createRequest( op, nr, outgoingData );
    }        
    
	if( !filterRequests_ ) {
		QNetworkReply* nr = QNetworkAccessManager::createRequest( op, req, outgoingData );
		connect( nr, SIGNAL( error( QNetworkReply::NetworkError ) ), 
			this, SLOT( OnReplyError( QNetworkReply::NetworkError ) ) ); 
		if( ignoreSSLErrors_ ) nr->ignoreSslErrors();
		return nr;
	}

	QString url = req.url().toString( QUrl::RemoveUserInfo | QUrl::StripTrailingSlash );  
    if( enableUrlMapping_ ) {
        for( RedirMap::const_iterator i = redirMap_.begin(); i != redirMap_.end(); ++i ) {
            if( i->rx.indexIn( url ) > -1 ) {
                QString u = i->url;
                for( int c = 0; c != i->rx.captureCount(); ++c ) {
                    u = u.replace( QString( "$%1" ).arg( c ), i->rx.cap( c ) );
                }
                url = u;
                break;    
            }
        } 
    }            

    for( RegExps::const_iterator i = deny_.begin(); i != deny_.end(); ++i ) {
        if( i->exactMatch( url ) ) {
            if( signalAccessDenied_ ) emit UrlAccessDenied( url );
            QNetworkRequest nr;
	        nr.setUrl( defaultUrl_ );
            return QNetworkAccessManager::createRequest( op, nr, outgoingData );
        }
    }
    for( RegExps::const_iterator i = allow_.begin(); i != allow_.end(); ++i ) {
        if( i->exactMatch( url ) ) {
			QNetworkReply* nr = QNetworkAccessManager::createRequest( op, req, outgoingData );
		    connect( nr, SIGNAL( error( QNetworkReply::NetworkError ) ), 
			    this, SLOT( OnReplyError( QNetworkReply::NetworkError ) ) ); 
		    if( ignoreSSLErrors_ ) nr->ignoreSslErrors();
            return nr;
        }
    }

    if( signalAccessDenied_ ) emit UrlAccessDenied( url );
    QNetworkRequest nr;
	nr.setUrl( defaultUrl_ );
    return QNetworkAccessManager::createRequest( op, nr, outgoingData );   
}

//slots
void NetworkAccessManager::OnReplyError( QNetworkReply::NetworkError ) {
	QNetworkReply* nr = qobject_cast< QNetworkReply* >( QObject::sender() );
	if( nr ) emit OnError( nr->errorString() );
	else emit OnError( "NETWORK ERROR" );
}

void NetworkAccessManager::OnAuthenticateRequest( QNetworkReply*, QAuthenticator* ) {
	std::cout << "Authentication requested" << std::endl;

}

#ifndef QT_NO_OPENSSL
void NetworkAccessManager::OnSSLErrors( QNetworkReply*, const QList< QSslError >& errors ) {
	QStringList sl;
	for( QList< QSslError >::const_iterator err = errors.begin();
		 err != errors.end(); ++err	 ) {
		sl.append( err->errorString() );
	}
	emit OnError( sl.join( "\n" ) );
}
#endif


}
