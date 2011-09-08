//#SRCHEADER
#include "LocoContext.h"
#include "LocoFileSystem.h"

namespace loco {

QVariant FileSystem::fopen( const QString& fname, const QStringList& mode ) {    
    if( GetContext() == 0 ) Error( "NULL Context" );
    File* f = new File;
    QVariant obj = GetContext()->AddObjToJSContext( f );
    f->open( fname, mode );
    return obj;
}

QVariant FileSystem::dir( const QString& dir ) {    
    if( GetContext() == 0 ) Error( "NULL Context" );
    Dir* d = new Dir;   
    QVariant obj = GetContext()->AddObjToJSContext( d );
    if( !dir.isEmpty() ) d->setPath( dir );
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

}