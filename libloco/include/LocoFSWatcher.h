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

