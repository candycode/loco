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

}