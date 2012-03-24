#pragma once
//#SRCHEADER

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
    QByteArray fread( const QString& fname ) const;
    bool fwrite( const QString& fname, const QByteArray& data, bool append = false ) const;
    bool fwrite( const QString& fname, const QString& data, bool append = false ) const {
    	return this->fwrite( fname, QByteArray( data.toAscii() ), append );
    }
    QVariant fopen( const QString& fname, const QStringList& mode ) const;
    QVariant dir( const QString& dirPath = "" ) const;
    QVariant watcher( const QString& path = "" ) const;
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
