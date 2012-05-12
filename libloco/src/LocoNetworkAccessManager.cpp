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


#include "LocoNetworkAccessManager.h"

#include <QDate>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QSettings>
#include <QNetworkDiskCache>
#ifdef LOCO_GUI
#include <QtGui/QDesktopServices>
#endif
#include <QSslConfiguration>
#include <QAuthenticator>

#include <stdexcept>

// some SSL-related code taken from Arora

namespace loco {


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
        settings.endGroup();
        //settings.sync();
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
       settings.endGroup();
       return cookieList;
   }
private:
    QString cookieFile_;
};

NetworkAccessManager::NetworkAccessManager( QObject* p,
                                            bool cache,
                                            const QString cacheDir )
    : QNetworkAccessManager( p ), 
      rxPattern_( QRegExp::RegExp2 ),
      filterRequests_( true ),
      allowNetAccess_( false ),
      enableUrlMapping_( false ),
      signalAccessDenied_( true ),
      logRequests_( false ),
      emitRequestSignal_( false ),
      customRequestHandlingEnabled_( false ) {
    //setCookieJar( new NetworkCookieJar ); // with this gmail doesn't work properly
    if( cache ) {
        networkDiskCache_ = new QNetworkDiskCache( this );
        if( !cacheDir.isEmpty() ) {
            networkDiskCache_->setCacheDirectory( cacheDir );
        } else {
#ifdef LOCO_GUI
            networkDiskCache_->setCacheDirectory(
                    QDesktopServices::storageLocation(QDesktopServices::CacheLocation));
#else
            networkDiskCache_->setCacheDirectory( "./" );
#endif
        }
        setCache( networkDiskCache_ );
    }
    connect( this, SIGNAL( authenticationRequired( QNetworkReply*, QAuthenticator* ) ),
             SLOT( OnAuthenticateRequest( QNetworkReply*,QAuthenticator* ) ) );
    connect( this, SIGNAL( sslErrors( QNetworkReply*, const QList< QSslError >& ) ),
             this, SLOT( OnSSLErrors( QNetworkReply*, const QList< QSslError >& ) ) );
    LoadSettings();
        
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
            if( !m.contains( i->first ) ) m[ i->first ] = QVariantList();
            QVariantList vl =  m[ i->first ].toList();
            vl.push_back( i->second );
            m[ i->first ] = vl;
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
        return HandleRequest( op, nr, outgoingData );
    }        
    
    if( !filterRequests_ ) {
        QNetworkReply* nr = HandleRequest( op, req, outgoingData );
        connect( nr, SIGNAL( error( QNetworkReply::NetworkError ) ),
                 this, SLOT( OnReplyError( QNetworkReply::NetworkError ) ) );
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
            return HandleRequest( op, nr, outgoingData );
        }
    }
    for( RegExps::const_iterator i = allow_.begin(); i != allow_.end(); ++i ) {
        if( i->exactMatch( url ) ) {
            QNetworkReply* nr = HandleRequest( op, req, outgoingData );
            //connect( nr, SIGNAL( error( QNetworkReply::NetworkError ) ),
            //    this, SLOT( OnReplyError( QNetworkReply::NetworkError ) ) );
            return nr;
        }
    }

    if( signalAccessDenied_ ) emit UrlAccessDenied( url );
    QNetworkRequest nr;
    nr.setUrl( defaultUrl_ );
    return HandleRequest( op, nr, outgoingData );
}


void NetworkAccessManager::LoadSettings() {
///@todo add configuration for proxy
#ifndef QT_NO_OPENSSL
    QSettings settings;
    //settings.beginGroup( "SSL" );
    QSslConfiguration sslCfg = QSslConfiguration::defaultConfiguration();
    QList<QSslCertificate> ca_list = sslCfg.caCertificates();
    QList<QSslCertificate> ca_new = QSslCertificate::fromData(settings.value(QLatin1String("CaCertificates")).toByteArray());
    ca_list += ca_new;
    sslCfg.setCaCertificates(ca_list);
    //sslCfg.setProtocol(QSsl::AnyProtocol);
    QSslConfiguration::setDefaultConfiguration(sslCfg);
    //settings.endGroup();
#endif
}

//slots
void NetworkAccessManager::OnReplyError( QNetworkReply::NetworkError ) {
    QNetworkReply* nr = qobject_cast< QNetworkReply* >( QObject::sender() );
    if( nr ) emit OnError( nr->errorString() );
    else emit OnError( "NETWORK ERROR" );
}

void NetworkAccessManager::OnAuthenticateRequest( QNetworkReply* reply, QAuthenticator* a ) {
    if( authenticator_.isNull()  ) return;
    QVariantMap credentials = authenticator_->Credentials( reply->url().toString() );
    if( !credentials.isEmpty() ) {
        a->setUser( credentials[ "user" ].toString() );
        a->setPassword( credentials[ "password" ].toString() );
    }
}


#ifndef QT_NO_OPENSSL
QString CertToFormattedString( const QSslCertificate& cert ) {
    QStringList message;
    message << cert.subjectInfo( QSslCertificate::CommonName );
    message << QString("Issuer: %1").arg(cert.issuerInfo( QSslCertificate::CommonName ) );
    message << QString("Not valid before: %1").arg( cert.effectiveDate().toString());
    message << QString("Valid until: %1").arg( cert.expiryDate().toString() );

    QMultiMap<QSsl::AlternateNameEntryType, QString> names = cert.alternateSubjectNames();
    if (names.count() > 0) {
        QString list;
        list += ("Alternate Names:");
        list += QStringList(names.values(QSsl::DnsEntry)).join(QLatin1String(", "));
        list += QLatin1String("\n");
        message << list;
    }
    QString result = message.join(QLatin1String("\n"));
    return result;
}


void NetworkAccessManager::OnSSLErrors( QNetworkReply* reply, const QList< QSslError >& errors ) {
    QSettings settings;
    //settings.beginGroup( "SSL" );
    QList< QSslCertificate > ca_merge = QSslCertificate::fromData(settings.value(QLatin1String("CaCertificates")).toByteArray());
    //settings.endGroup();
    QList< QSslCertificate > ca_new;
    QStringList errorStrings;
    for( int i = 0; i < errors.count(); ++i ) {
        if( ca_merge.contains( errors.at( i ).certificate() ) ) continue;
        errorStrings += errors.at( i ).errorString();
        if( !errors.at(i).certificate().isNull() ) {
            ca_new.append( errors.at( i ).certificate() );
        }
    }
    if( errorStrings.isEmpty() ) {
        reply->ignoreSslErrors();
        return;
    }
    if( sslHandler_.isNull()  ) return;
      bool ret = sslHandler_->Check( reply->url().toString(), errorStrings );
    if( ret ) {
        if( ca_new.count() > 0 ) {
            QStringList certinfos;
            for ( int i = 0; i < ca_new.count(); ++i ) certinfos += CertToFormattedString( ca_new.at( i ) );
            ret = sslHandler_->AcceptCertificates( certinfos );
            if( ret  ) {
                ca_merge += ca_new;
                QSslConfiguration sslCfg = QSslConfiguration::defaultConfiguration();
                QList<QSslCertificate> ca_list = sslCfg.caCertificates();
                ca_list += ca_new;
                sslCfg.setCaCertificates(ca_list);
                //sslCfg.setProtocol(QSsl::AnyProtocol);
                QSslConfiguration::setDefaultConfiguration(sslCfg);
                reply->setSslConfiguration(sslCfg);
                QByteArray pems;
                for( int i = 0; i < ca_merge.count(); ++i ) pems += ca_merge.at(i).toPem() + '\n';
                //settings.beginGroup( "SSL" );
                settings.setValue( QLatin1String( "CaCertificates" ), pems );
                //settings.endGroup();
            }
        }
        reply->ignoreSslErrors();
    }
    //else emit OnSSLError( errorStrings.join( "\n" ) );
}
#endif


}
