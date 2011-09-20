#pragma once
//#SRCHEADER

#include <QCoreApplication>
#include <QDir>
#include <QString>
#include <QList>
#include <QRegExp>


namespace loco {

struct FileAccessEntry {
    QRegExp rx;
    QIODevice::OpenMode mode;
    FileAccessEntry() : mode( QIODevice::NotOpen ) {}
    FileAccessEntry( const QRegExp& r, QIODevice::OpenMode m ) : rx( r ), mode( m ) {}
};


typedef QList< FileAccessEntry > FileAccessEntries; 

class FileAccessManager : public QObject {
public:
    FileAccessManager( QObject* p = 0 ) 
      : QObject( p ), 
        rxPattern_( QRegExp::RegExp2 ),
        filterAccess_( true ),
        allowFileAccess_( false ) {}
    QRegExp::PatternSyntax GetRxPatternSyntax() const { return rxPattern_; }
    void SetRxPatternSyntax( QRegExp::PatternSyntax ps ) { rxPattern_ = ps; }
    bool GetFilterAccess() const { return filterAccess_; }
    void SetFilterAccess( bool fr ) { filterAccess_ = fr; } 
    bool AddAllowRule( const QRegExp& rx, QIODevice::OpenMode mode ) { 
        if( !rx.isValid() ) return false;
        allow_.push_back( FileAccessEntry( rx, mode ) );
        return true; 
    }
    bool AddDenyRule( const QRegExp& rx, QIODevice::OpenMode mode ) { 
        if( !rx.isValid() ) return false;
        deny_.push_back( FileAccessEntry( rx, mode ) );
        return true; 
    }
    void ResetRules() { deny_.clear(); allow_.clear(); }
    const FileAccessEntries& GetDenyRules() const { return deny_; }
    const FileAccessEntries& GetAllowRules() const { return allow_; }
    bool GetAllowFileAccess() const { return allowFileAccess_; }
    void SetAllowFileAccess( bool na ) { allowFileAccess_ = na; }
	bool CheckAccess( QString filePath, QIODevice::OpenMode openMode = QIODevice::ReadWrite ) const {
        if( !allowFileAccess_ ) return false;
        if( !filterAccess_ ) return true;  
        const QString path = filePath
                             .replace( "$HOME", QDir::home().absolutePath() )
                             .replace( "$APPDIR", QCoreApplication::applicationDirPath() );
         
        for( FileAccessEntries::const_iterator i = deny_.begin(); i != deny_.end(); ++i ) {
            if( i->rx.exactMatch( path ) && ( openMode & i->mode ) ) {
                return false;
            }
        }
        for( FileAccessEntries::const_iterator i = allow_.begin(); i != allow_.end(); ++i ) {
            if( i->rx.exactMatch( path ) && ( openMode & i->mode ) ) {
                return true;
            }
        }
        return false;
    }
private:
    QRegExp::PatternSyntax rxPattern_;
    bool filterAccess_; 
    bool allowFileAccess_;
    FileAccessEntries allow_;
    FileAccessEntries deny_;     
};

}