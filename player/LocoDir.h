#pragma once
//#SRCHEADER

#include <QString>
#include <QStringList>
#include <QDir>
#include <QFileInfoList>
#include <QVariantMap>
#include <QDateTime>

#include <iostream>

#include "LocoObject.h"

namespace loco {

class Dir : public Object {
    Q_OBJECT

public:
   Dir() : Object( 0, "LocoDir", "Loco/System/Dir" ) {}

public slots:

	void setPath( const QString& path ) { dir_.setPath( path ); }          
    bool cd( const QString& dirName )   { return dir_.cd( dirName ); }
    bool cdUp()                         { return dir_.cdUp(); }
    uint count () const                 { return dir_.count(); }
    QString dirName() const             { return dir_.dirName(); }
    QVariantMap entries( const QStringList& filters = QStringList() ) const {
        return MakeEntries( filters );
    }
    bool exists() const                 { return dir_.exists(); }  

private:

    QVariantMap MakeEntries( const QStringList& filters ) const {
        QFileInfoList fi = dir_.entryInfoList( filters );
        QVariantMap e;
        for( QFileInfoList::const_iterator i = fi.begin(); i != fi.end(); ++i ) {
            QVariantMap p;
            p[ "dir"  ] = i->isDir();
            p[ "file" ] = i->isFile();
            p[ "bundle" ] = i->isBundle(); // mac
            p[ "executable" ] = i->isExecutable();
            p[ "hidden" ] = i->isHidden();
            p[ "readable" ] = i->isReadable();
            p[ "writable" ] = i->isWritable();
            p[ "symlink" ] = i->isSymLink();
            p[ "last_modified" ] = i->lastModified();
            p[ "last_read" ] = i->lastRead();
            p[ "absolute_path" ] = i->absoluteFilePath();
            e[ i->fileName() ] = p;
        }
        return e;
    }    
       
private:
    QDir dir_;
};

}