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
        if( sortFlags.isEmpty() ) return  QDir::NoSort;
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
/*
QDir::Dirs	0x001	List directories that match the filters.
QDir::AllDirs	0x400	List all directories; i.e. don't apply the filters to directory names.
QDir::Files	0x002	List files.
QDir::Drives	0x004	List disk drives (ignored under Unix).
QDir::NoSymLinks	0x008	Do not list symbolic links (ignored by operating systems that don't support symbolic links).
QDir::NoDotAndDotDot	0x1000	Do not list the special entries "." and "..".
QDir::NoDot	0x2000	Do not list the special entry ".".
QDir::NoDotDot	0x4000	Do not list the special entry "..".
QDir::AllEntries	Dirs | Files | Drives	List directories, files, drives and symlinks (this does not list broken symlinks unless you specify System).
QDir::Readable	0x010	List files for which the application has read access. The Readable value needs to be combined with Dirs or Files.
QDir::Writable	0x020	List files for which the application has write access. The Writable value needs to be combined with Dirs or Files.
QDir::Executable	0x040	List files for which the application has execute access. The Executable value needs to be combined with Dirs or Files.
QDir::Modified	0x080	Only list files that have been modified (ignored on Unix).
QDir::Hidden	0x100	List hidden files (on Unix, files starting with a ".").
QDir::System	0x200	List system files (on Unix, FIFOs, sockets and device files are included; on Windows, .lnk files are included)
QDir::CaseSensitive	0x800	The filter should be case sensitive.

*/
