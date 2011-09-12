#pragma once
//#SRCHEADER

#include <QCoreApplication>
#include <QDir>
#include <QString>
#include <QList>
#include <QRegExp>


namespace loco {

typedef QList< QRegExp > RegExps; 

class FileAccessManager : public QObject {
public:
    FileAccessManager( QObject* p = 0 ) 
      : QObject( p ), 
        rxPattern_( QRegExp::RegExp2 ),
        filterRequests_( true ),
        allowFileAccess_( false ) {}
    QRegExp::PatternSyntax GetRxPatternSyntax() const { return rxPattern_; }
    void SetRxPatternSyntax( QRegExp::PatternSyntax ps ) { rxPattern_ = ps; }
    bool GetFilterAccess() const { return filterRequests_; }
    void SetFilterAccess( bool fr ) { filterRequests_ = fr; } 
    void AddAllowRule( const QRegExp& rx ) { allow_.push_back( rx ); }
    void AddDenyRule( const QRegExp& rx ) { deny_.push_back( rx ); }
    void ResetRules() { deny_.clear(); allow_.clear(); }
    const RegExps& GetDenyRules() const { return deny_; }
    const RegExps& GetAllowRules() const { return allow_; }
    bool GetAllowFileAccess() const { return allowFileAccess_; }
    void SetAllowFileAccess( bool na ) { allowFileAccess_ = na; }
	bool CheckAccess( QString filePath, QIODevice::OpenMode openMode = QIODevice::ReadWrite ) const {
        if( !allowFileAccess_ ) return false;
        if( !filterRequests_ ) return true;  
        const QString path = filePath
                             .replace( "$HOME", QDir::home().absolutePath() )
                             .replace( "$APPDIR",  QCoreApplication::applicationDirPath() );
        for( RegExps::const_iterator i = deny_.begin(); i != deny_.end(); ++i ) {
            if( i->indexIn( path ) > -1 ) {
                return false;
            }
        }
        for( RegExps::const_iterator i = allow_.begin(); i != allow_.end(); ++i ) {
            if( i->indexIn( path ) > -1 ) {
                return true;
            }
        }
        return false;
    }
private:
    QRegExp::PatternSyntax rxPattern_;
    bool filterRequests_; 
    bool allowFileAccess_;
    RegExps allow_;
    RegExps deny_;     
};

}