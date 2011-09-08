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

}