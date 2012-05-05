//#SRCHEADER
#include "LocoFile.h"
#include "LocoContext.h"

namespace loco {

bool File::open( const QStringList& openMode ) {
	if( GetContext() == 0 || GetContext()->GetFileAccessManager() == 0 ) {
        error( "FileAccessManager not available, aborting open operation" );
		return false;
	}
    const FileAccessManager* fm = GetContext()->GetFileAccessManager();
	if( !fm->CheckAccess( file_.fileName(), MapOpenMode( openMode ) ) ) {
		error( "Not authorized to access file " + file_.fileName() );
		return false;
	} else if( file_.fileName().isEmpty() ) {
        error( "Empty file name" );
        return false; 
    } else if( file_.open( MapOpenMode( openMode ) ) ) {
        return true;
    } else {
        error( "File " + file_.fileName() + " open failed" );
        return false;
    }            
}

QIODevice::OpenMode File::MapOpenMode( const QStringList& mode ) {
    QIODevice::OpenMode om = 0;
    for( QStringList::const_iterator i = mode.begin(); i != mode.end(); ++i ) {
        const QString& m = *i;
        if( m == "r" ) om |= QIODevice::ReadOnly;
        else if( m == "w" ) om |= QIODevice::WriteOnly;
        else if( m == "a" ) om |= QIODevice::Append;
        else if( m == "truncate" ) om |= QIODevice::Truncate;
        else if( m == "text" ) om |= QIODevice::Text;
        else if( m == "unbuffered" ) om |= QIODevice::Unbuffered;
    }
    return om;   
} 

}
