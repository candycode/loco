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

#include "LocoContext.h"
#include "LocoFileSystem.h"
#include "LocoFSWatcher.h"
#include "LocoTempFile.h"
#include "LocoFile.h"
#include "LocoDir.h"
#include "LocoFileAccessManager.h"

namespace loco {

QString FileSystem::fread( const QString& fname ) const {
    QFile f( fname );
    f.open( QIODevice::ReadOnly );
    QByteArray data;
    if( f.error() != QFile::NoError ) {
    	error( f.errorString() );
    	return data;
    }
    data = f.readAll();
    if( f.error() != QFile::NoError ) {
       	error( f.errorString() );
    }
    return data;
}

bool FileSystem::fwrite( const QString& fname,
		                 const QByteArray& data,
		                 bool append ) const {
	QFile f(fname);
	f.open( ( append ? QIODevice::WriteOnly | QIODevice::Append : QIODevice::WriteOnly ) );
	if( f.error() != QFile::NoError ) {
	    error( f.errorString() );
	    return false;
	}
    f.write( data );
    if( f.error() != QFile::NoError ) {
        error( f.errorString() );
    	return false;
    }
    return true;
}

QVariant FileSystem::fopen( const QString& fname, const QStringList& mode ) const {
    if( GetContext() == 0 ) {
        error( "NULL Context" );
        return QVariant();
    }
    File* f = new File( GetContext() );
    if( f->open( fname, mode ) ) {
        return GetContext()->AddObjToJSContext( f );
    } else {
        delete f;
        return QVariant();
    }
}

QVariant FileSystem::dir( const QString& dir ) const {
    if( GetContext() == 0 ) {
        error( "NULL Context" );
        return QVariant();
    }
    Dir* d = new Dir;   
    QVariant obj = GetContext()->AddObjToJSContext( d );
    if( !dir.isEmpty() ) d->setPath( dir );
    return obj;
}

QVariant FileSystem::watcher( const QString& path  ) const {
    if( GetContext() == 0 ) {
        error( "NULL Context" );
        return QVariant();
    }
    FSWatcher* f = new FSWatcher;   
    QVariant obj = GetContext()->AddObjToJSContext( f );
    if( !path.isEmpty() ) f->addPath( path );
    return obj;
}


QVariantMap FileSystem::drives() const {
    return Dir::MakeEntries( QDir::drives() );
}


QVariantMap FileSystem::MapPermissions( QFile::Permissions fp ) const {
    QVariantMap pm;
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

QVariant FileSystem::tmpFile( const QString& templateName ) const {
    if( GetContext() == 0 ) {
	    error( "NULL Context" );
	    return QVariant();
	}
    if( GetContext()->GetFileAccessManager() == 0 ) {
        error( "FileAccessManager not available, aborting open operation" );
    	return QVariant();
    }
    TempFile* f = new TempFile( templateName );
    const FileAccessManager* fm = GetContext()->GetFileAccessManager();
    if( !fm->CheckAccess( f->fileName(), QIODevice::ReadWrite ) ) {
        error( "Not authorized to access file " + f->fileName() );
    	return QVariant();
    }
	QVariant obj = GetContext()->AddObjToJSContext( f );
	if( !f->open() ) {
		delete f;
		error( "Cannot open temporary file " + f->fileName() );
		return QVariant();
	}
	return obj;
}

QVariant FileSystem::stdIn() const {
    File* f = new File( GetContext() );
    if( !f->Open( stdin, QFile::ReadOnly ) ) return QVariant();
    return GetContext()->AddObjToJSContext( f );
}

}
