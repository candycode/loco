#pragma once
//#SRCHEADER

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QString>
#include <QList>
#include <QStringList>
#include <QRegExp>

namespace loco {

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
    NetworkAccessManager( QObject* p = 0 ) 
      : QNetworkAccessManager( p ), 
        rxPattern_( QRegExp::RegExp2 ),
        filterRequests_( true ),
        allowNetAccess_( false ),
        enableUrlMapping_( false ) {}
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
protected:
    virtual QNetworkReply* createRequest( Operation op,
                                          const QNetworkRequest& req,
                                          QIODevice* outgoingData = 0 ) {
	
        if( !allowNetAccess_ ) {
            emit UnauthorizedNetworkAccessAttempt();
			QNetworkRequest nr;
		    nr.setUrl(QString(""));
            return QNetworkAccessManager::createRequest( op, nr, outgoingData );
        }        
        
        if( !filterRequests_ )
            return QNetworkAccessManager::createRequest( op, req, outgoingData );

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
                emit UrlAccessDenied( url );
                QNetworkRequest nr;
		        nr.setUrl(QString(""));
                return QNetworkAccessManager::createRequest( op, nr, outgoingData );
            }
        }
        for( RegExps::const_iterator i = allow_.begin(); i != allow_.end(); ++i ) {
            if( i->exactMatch( url ) ) {
                return QNetworkAccessManager::createRequest( op, req, outgoingData );
            }
        }

        emit UrlAccessDenied( url );
        QNetworkRequest nr;
		nr.setUrl(QString(""));
        return QNetworkAccessManager::createRequest( op, nr, outgoingData );   
    }
signals:
    void UrlAccessDenied( QString );
    void UnauthorizedNetworkAccessAttempt();

private:
    QNetworkAccessManager nam_;
    QRegExp::PatternSyntax rxPattern_;
    bool filterRequests_; 
    bool allowNetAccess_;
    RegExps allow_;
    RegExps deny_;
    bool enableUrlMapping_;
    RedirMap redirMap_;     

};

}