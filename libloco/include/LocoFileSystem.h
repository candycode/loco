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


#include <QString>
#include <QFile>
#include <QDir>
#include <QMap>
#include <QStringList>

#include "LocoObject.h"


namespace loco {

class FileSystem : public Object {
    Q_OBJECT

public:
   FileSystem() : Object( 0, "LocoFileSystem", "Loco/System/FileSystem" ) {}

public slots:
    QVariant stdIn() const;
    QVariant tmpFile( const QString& = QString() ) const;
    QString fread( const QString& fname ) const;
    bool fwrite( const QString& fname, const QByteArray& data, bool append = false ) const;
    bool fwrite( const QString& fname, const QString& data, bool append = false ) const {
        return this->fwrite( fname, QByteArray( data.toAscii() ), append );
    }
    QVariant fopen( const QString& fname, const QStringList& mode ) const;
    QVariant dir( const QString& dirPath = "." ) const;
    QVariant watcher( const QString& path = "." ) const;
    bool copy( const QString& f1, const QString& f2 ) const { return QFile::copy( f1, f2); }
    bool fexists( const QString& f ) const { 
        return QFile::exists( f ) && QFileInfo( f ).isFile();
    }
    bool dexists( const QString& dir ) const { 
        QDir d;
        d.setPath( dir );
        return d.exists() && QFileInfo( dir ).isDir();
    }
    bool exists( const QString& f ) { return QFile::exists( f ); }
    bool link( const QString& target, const QString& lnk ) const { return QFile::link( target, lnk ); }
    bool remove( const QString& f ) const { return QFile::remove( f ); }
    bool rename( const QString& target, const QString& newName ) const { return QFile::rename( target, newName ); }
    QString symLinkTarget( const QString& f ) const { return QFile::symLinkTarget( f ); } 
    QVariantMap permissions( const QString& f ) const {
        return MapPermissions( QFile::permissions( f ) );
    } 
    QString home() const { return QDir::homePath(); }
    QString root() const { return QDir::rootPath(); }
    QString curDir() const { return QDir::currentPath(); }
    QString tempPath() const { return QDir::tempPath(); }
    QChar separator() const { return QDir::separator(); }
    QVariantMap drives() const; //windows
    QString toNativeSeparators( const QString& inpath ) const {
        return QDir::toNativeSeparators( inpath );
    }
    QString fromNativeSeparators( const QString& inpath ) const {
        return QDir::fromNativeSeparators( inpath );
    }        
    bool mkdir( const QString& name ) const {
        QDir d;
        return d.mkdir( name );
    }
    bool mkpath( const QString& pname ) const {
        QDir d;
        return d.mkpath( pname );
    }
    QString readAll( const QString& fpath ) const {
        QFile f( fpath );
        f.open( QIODevice::ReadOnly );
        //if( !f.errorString().isEmpty() ) 
        return f.readAll();
    } 
private:
    QVariantMap MapPermissions( QFile::Permissions fp ) const;

};

}
