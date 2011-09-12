#pragma once
//#SRCHEADER

#include <QString>
#include <QFile>
#include <QDir>
#include <QMap>
#include <QStringList>

#include "LocoFile.h"
#include "LocoDir.h"
#include "LocoObject.h"


namespace loco {

class FileSystem : public Object {
    Q_OBJECT

public:
   FileSystem() : Object( 0, "LocoFileSystem", "Loco/System/FileSystem" ) {}

public slots:
    
    QVariant fopen( const QString& fname, const QStringList& mode );

    QVariant dir( const QString& dirPath = "" );

    QVariant watcher( const QString& path = "" );  

    bool copy( const QString& f1, const QString& f2 ) const { return QFile::copy( f1, f2); }

    bool fexists( const QString& f ) const { 
        return QFile::exists( f ) && QFileInfo( f ).isFile();
    }

    bool dexists( const QString& dir ) const { 
        QDir d;
        d.setPath( dir );
        return d.exists() && QFileInfo( dir ).isDir();
    }

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
        

private:
    QVariantMap MapPermissions( QFile::Permissions fp ) const;

};

}