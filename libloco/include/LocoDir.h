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
    QVariantMap entries( const QStringList& nameFilters = QStringList(),
                         const QStringList& filters = QStringList(),
                         const QStringList& sortFlags = QStringList() ) const {
        QFileInfoList fi = dir_.entryInfoList( nameFilters,
                               MakeFilters( filters ),
                               MakeSortFlags( sortFlags ) );
        return MakeEntries( fi );
    }
    bool exists() const                 { return dir_.exists(); }  

private:
    friend class FileSystem;
    static QVariantMap MakeEntries( QFileInfoList fi ) {
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
    static QDir::Filters MakeFilters( const QStringList& filters ) {
        if( filters.isEmpty() ) return  QDir::NoFilter;
        QDir::Filters f = 0; 
        for( QStringList::const_iterator i = filters.begin();
             i != filters.end(); ++i ) {
            const QString& s = *i;
            if( s == "dirs"          ) f |= QDir::Dirs;
            else if( s == "alldirs"  ) f |= QDir::AllDirs;
            else if( s == "files"    ) f |= QDir::Files;
            else if( s == "drives"   ) f |= QDir::Drives;
            else if( s == "nosymlnk" ) f |= QDir::NoSymLinks;
            else if( s == "noddd"    ) f |= QDir::NoDotAndDotDot;
            else if( s == "nd"       ) f |= QDir::NoDot;
            else if( s == "ndd"      ) f |= QDir::NoDotDot;
            else if( s == "all"      ) f |= QDir::AllEntries;
            else if( s == "read"     ) f |= QDir::Readable;
            else if( s == "write"    ) f |= QDir::Writable;
            else if( s == "exe"      ) f |= QDir::Executable;
            else if( s == "mod"      ) f |= QDir::Modified;
            else if( s == "hidden"   ) f |= QDir::Hidden;
            else if( s == "sys"      ) f |= QDir::System;
            else if( s == "case"     ) f |= QDir::CaseSensitive;
        }
        return f;
    } 
    static QDir::SortFlags MakeSortFlags( const QStringList& sortFlags ) {
        if( sortFlags.isEmpty() ) return QDir::NoSort;
        QDir::SortFlags f = 0;
        for( QStringList::const_iterator i = sortFlags.begin();
             i != sortFlags.end(); ++i ) {
            const QString& s = *i;
            if( s == "name"          ) f |= QDir::Name;
            else if( s == "time"     ) f |= QDir::Time;
            else if( s == "tize"     ) f |= QDir::Size;
            else if( s == "type"     ) f |= QDir::Type;
            else if( s == "unsorted" ) f |= QDir::Unsorted;
            else if( s == "nosort"   ) f |= QDir::NoSort;
            else if( s == "dirfirst" ) f |= QDir::DirsFirst;
            else if( s == "dirlast"  ) f |= QDir::DirsLast;
            else if( s == "reverse"  ) f |= QDir::Reversed;
            else if( s == "nocase"   ) f |= QDir::IgnoreCase;
            else if( s == "loc"      ) f |= QDir::LocaleAware;
        }
        return f;  
    }
       
private:
    QDir dir_;
};

}

