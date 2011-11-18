//#SRCHEADER
#include "LocoProcess.h"
#include "LocoContext.h"

namespace loco {
void Process::setStandardOutputProcess ( QObject* dest ) {
    if( GetContext() == 0 ) {
        error( "NULL Context" );
        return; 
    }
    Process* p = qobject_cast< Process* >( dest );
    if( p == 0 ) {
        error( "Wrong object type" );
        return; 
    }
    p_.setStandardOutputProcess( &p->GetProcess() );  
}
}
