//#SRCHEADER
#include "LocoProcess.h"
#include "LocoContext.h"

namespace loco {
void Process::setStandardOutputProcess ( const QVariantMap& dest ) {
    if( GetContext() == 0 ) {
        error( "NULL Context" );
        return; 
    }
    Object* obj = GetContext()->Find( dest[ "jsInstance" ].toString() );
    if( obj == 0 ) {
        error( "Process object not found" );
        return;
    } 
    Process* p = qobject_cast< Process* >( obj );
    if( p == 0 ) {
        error( "Wrong object type: " + obj->type() );
        return; 
    }
    p_.setStandardOutputProcess( &p->GetProcess() );  
}
}