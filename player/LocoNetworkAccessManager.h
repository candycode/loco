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

class NetworkAccessManager : public QNetworkAccessManager {
    Q_OBJECT
public:
    NetworkAccessManager( QObject* p = 0 ) 
      : QNetworkAccessManager( p ), 
        rxPattern_( QRegExp::RegExp2 ),
        filterRequests_( true ),
        allowNetAccess_( false ) {}
    QRegExp::PatternSyntax GetRxPatternSyntax() const { return rxPattern_; }
    void SetRxPatternSyntax( QRegExp::PatternSyntax ps ) { rxPattern_ = ps; }
    bool GetFilterRequests() const { return filterRequests_; }
    void SetFilterRequests( bool fr ) { filterRequests_ = fr; } 
    void AddAllowRule( const QRegExp& rx ) { allow_.push_back( rx ); }
    void AddDenyRule( const QRegExp& rx ) { deny_.push_back( rx ); }
    void ResetRules() { deny_.clear(); allow_.clear(); }
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
            return 0;
        }        
        
        if( !filterRequests_ )
            return QNetworkAccessManager::createRequest( op, req, outgoingData );

        const QString url = req.url().toString( QUrl::RemoveAuthority | QUrl::StripTrailingSlash );        
    
        for( RegExps::const_iterator i = deny_.begin(); i != deny_.end(); ++i ) {
            if( i->indexIn( url ) > -1 ) {
                emit NetworkRequestDenied( url );
                return 0;
            }
        }
        for( RegExps::const_iterator i = allow_.begin(); i != allow_.end(); ++i ) {
            if( i->indexIn( url ) > -1 ) {
                return QNetworkAccessManager::createRequest( op, req, outgoingData );
            }
        }

        emit NetworkRequestDenied( url );
        return 0;   
    }
signals:
    void NetworkRequestDenied( QString );
    void UnauthorizedNetworkAccessAttempt();

private:
    QNetworkAccessManager nam_;
    QRegExp::PatternSyntax rxPattern_;
    bool filterRequests_; 
    bool allowNetAccess_;
    RegExps allow_;
    RegExps deny_;     

};

}