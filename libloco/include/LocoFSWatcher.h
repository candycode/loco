#pragma once
//#SRCHEADER

#include <QString>
#include <QStringList>
#include <QFileSystemWatcher>

#include "LocoObject.h"

namespace loco {

//file system watcher

class FSWatcher : public Object {
    Q_OBJECT

public:
   FSWatcher() : Object( 0, "LocoFileSystemWatcher", "Loco/System/FileSystemWatcher" ) {
       connect( &fsw_, SIGNAL( directoryChanged( const QString& ) ),
                this, SIGNAL( dirChanged( const QString& ) ) ); 
       connect( &fsw_, SIGNAL( fileChanged( const QString& ) ),
                this, SIGNAL( fileChanged( const QString& ) ) ); 
}

public slots:
    void addPath( const QString& path ) { fsw_.addPath( path ); }
    void addPaths( const QStringList& paths ) { fsw_.addPaths( paths ); }
    QStringList dirs()  const { return fsw_.directories();  }
    QStringList files() const { return fsw_.files(); } 
    void removePath( const QString& path ) { fsw_.removePath( path ); }
    void removePaths( const QStringList& paths ) { fsw_.removePaths( paths ); }
    	
signals:
    void dirChanged( const QString& path );
    void fileChanged( const QString& file );
  
private:
    QFileSystemWatcher fsw_;
};

}

