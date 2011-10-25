#pragma once
//#SRCHEADER

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QString>
#include <QList>
#include <QVariantMap>
#include <QStringList>
#include <QRegExp>
#include <QNetworkReply>
#include <QPointer>

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

class NetworkAccessManager : public QNetworkAccessManager {
    Q_OBJECT
public:
    NetworkAccessManager( QObject* p = 0 bool cache = true,
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
	void SetIgnoreSSLErrors( bool yes ) { ignoreSSLErrors_ = yes; }
protected:
    virtual QNetworkReply* createRequest( Operation op,
                                          const QNetworkRequest& req,
                                          QIODevice* outgoingData = 0 );
private:
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
};

}
