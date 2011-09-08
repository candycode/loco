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

    QMap<QString, QStringList> permissions( const QString& f ) const {
        return MapPermissions( QFile::permissions( f ) );
    } 

private:
    QMap<QString, QStringList> MapPermissions( QFile::Permissions fp ) const {

        typedef QMap<QString, QStringList> PM;
        if( fp == 0 ) return PM();
        PM pm;
        QStringList sl;
        //owner
        if( QFile::ReadOwner  & fp ) sl << "r";
        if( QFile::WriteOwner & fp ) sl << "w";
        if( QFile::ExeOwner   & fp ) sl << "x";
        pm[ "owner" ] = sl;
        sl.clear();
        //user
        if( QFile::ReadUser  & fp ) sl << "r";
        if( QFile::WriteUser & fp ) sl << "w";
        if( QFile::ExeUser   & fp ) sl << "x";
        pm[ "user" ] = sl;
        sl.clear();
        //group
        if( QFile::ReadGroup  & fp ) sl << "r";
        if( QFile::WriteGroup & fp ) sl << "w";
        if( QFile::ExeGroup   & fp ) sl << "x";
        pm[ "group" ] = sl;
        sl.clear();
        //world
        if( QFile::ReadOther  & fp ) sl << "r";
        if( QFile::WriteOther & fp ) sl << "w";
        if( QFile::ExeOther   & fp ) sl << "x";
        pm[ "world" ] = sl;      
        return pm;
    }      
private:
    QFile file_;
};

}