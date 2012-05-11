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


#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QString>
#include <QList>
#include <QVariantMap>
#include <QStringList>
#include <QRegExp>
#include <QNetworkReply>
#include <QPointer>
#include <QMap>

#include "LocoIAuthenticator.h"
#include "LocoISSLExceptionHandler.h"
#include "LocoINetworkRequestHandler.h"

#ifndef QT_NO_OPENSSL
#include <QSslError>
#endif

class QNetworkDiskCache;

namespace loco {

struct IAuthenticator;
struct ISSLExceptionHandler;

typedef QList< QRegExp > RegExps; 

struct RedirEntry {
    QRegExp rx;
    QString url; // use $0, $1...to specify captures from regexp
    RedirEntry( const QRegExp& r, const QString u ) : rx( r ), url( u ) {}
    RedirEntry() {}  
};

typedef QList< RedirEntry > RedirMap;

typedef QMap< QString, INetworkRequestHandler* > RequestHandlers;

class NetworkAccessManager : public QNetworkAccessManager {
    Q_OBJECT
public:
    NetworkAccessManager( QObject* p = 0, bool cache = true,
                          const QString cacheDir = QString() );
    QRegExp::PatternSyntax GetRxPatternSyntax() const { return rxPattern_; }
    void SetRxPatternSyntax( QRegExp::PatternSyntax ps ) { rxPattern_ = ps; }
    bool GetFilterRequests() const { return filterRequests_; }
    void SetFilterRequests( bool fr ) { filterRequests_ = fr; } 
    void AddAllowRule( const QRegExp& rx ) { allow_.push_back( rx ); }
    void AddDenyRule( const QRegExp& rx ) { deny_.push_back( rx ); }
    void AddRedirMapping( const QRegExp& rx, const QString& url ) { redirMap_.push_back( RedirEntry( rx, url ) ); }
    void ResetRules() { deny_.clear(); allow_.clear(); }
    void ResetRedirMap() { redirMap_.clear(); }
    const RegExps& GetDenyRules() const { return deny_; }
    const RegExps& GetAllowRules() const { return allow_; }
    bool GetAllowNetAccess() const { return allowNetAccess_; }
    void SetAllowNetAccess( bool na ) { allowNetAccess_ = na; }
    void SetSignalAccessDenied( bool yes ) { signalAccessDenied_ = yes; }
    bool GetSignalAccessDenied() const { return signalAccessDenied_; }
    void SetDefaultUrl( const QString& durl ) { defaultUrl_ = durl; }
    const QString& GetDefaultUrl() const { return defaultUrl_; }
    void ResetRequests() { requests_.clear(); }
    const QList< QVariantMap >& Requests() const { return requests_; }
    void SetLogRequestsEnabled( bool yes ) { logRequests_ = yes; }
    void EmitRequestSignal( bool yes ) { emitRequestSignal_ = yes; }
    void SetAuthenticator( IAuthenticator* auth ) {
    	authenticator_ = auth;
    	if( !authenticator_.isNull() ) authenticator_->setParent( this );
    }
    void SetSSLExceptionHandler( ISSLExceptionHandler* sa ) {
    	sslHandler_ = sa;
    	if( !sslHandler_.isNull() ) sslHandler_->setParent( this );
    }
    void SetDefaultSSLExceptionHandler() { SetSSLExceptionHandler( new DefaultSSLExceptionHandler() ); }
    void SetDefaultAuthenticator( const QString& user, const QString& pwd ) {
    	if( dynamic_cast< DefaultAuthenticator* >( authenticator_.data() ) == 0 ) {
    		SetAuthenticator( new DefaultAuthenticator( user, pwd ) );
    	} else {
    		DefaultAuthenticator* auth = dynamic_cast< DefaultAuthenticator* >( authenticator_.data() );
    	    auth->setUser( user );
    	    auth->setPassword( pwd );
    	}
    }
    void RemoveAuthenticator()       { authenticator_->deleteLater(); authenticator_ = 0; }
    void RemoveSSLExceptionHandler() { sslHandler_->deleteLater(); sslHandler_ = 0; }
    // request handler runtime in managed by this object, objects are destroyed in the destructor
    void AddNetworkRequestHandler( const QString& scheme,
    		                       INetworkRequestHandler* nrh  ) {
    	requestHandlers_[ scheme ] = nrh;
    }
    ~NetworkAccessManager() {
    	for( RequestHandlers::iterator i = requestHandlers_.begin();
    		 i != requestHandlers_.end(); ++i ) {
    		delete i.value();
    	}
    }
    void SetEnableCustomRequestHandlers( bool yes ) {
    	customRequestHandlingEnabled_ = yes;
    }
protected:
    virtual QNetworkReply* createRequest( Operation op,
                                          const QNetworkRequest& req,
                                          QIODevice* outgoingData = 0 );
private:
    QNetworkReply* HandleRequest( Operation op,
                                  const QNetworkRequest& req,
                                  QIODevice* outgoingData = 0 ) {
    	if( customRequestHandlingEnabled_ && requestHandlers_.contains( req.url().scheme() ) ) {
    		return requestHandlers_[ req.url().scheme() ]->HandleRequest( op, req, outgoingData );
    	} else {
    		return QNetworkAccessManager::createRequest( op, req, outgoingData );
    	}
    }
    void LoadSettings();
signals:
    void UrlAccessDenied( QString );
    void UnauthorizedNetworkAccessAttempt();
    void OnRequest( const QVariantMap& );
	void OnError( const QString& ); 
private slots:
    void OnReplyError( QNetworkReply::NetworkError );
	void OnAuthenticateRequest( QNetworkReply*, QAuthenticator* );
#ifndef QT_NO_OPENSSL
	void OnSSLErrors( QNetworkReply*, const QList< QSslError >& errors );
#endif
private:
    QNetworkAccessManager nam_;
    QRegExp::PatternSyntax rxPattern_;
    bool filterRequests_; 
    bool allowNetAccess_;
    RegExps allow_;
    RegExps deny_;
    bool enableUrlMapping_;
    RedirMap redirMap_;
    bool signalAccessDenied_;
    QString defaultUrl_;
    bool logRequests_;
    QList< QVariantMap > requests_;
    bool emitRequestSignal_;
	QNetworkDiskCache* networkDiskCache_;
    QPointer< IAuthenticator > authenticator_;
    QPointer< ISSLExceptionHandler  > sslHandler_;
    RequestHandlers requestHandlers_;
    bool customRequestHandlingEnabled_;
};

}
