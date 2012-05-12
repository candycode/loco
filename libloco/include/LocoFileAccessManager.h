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