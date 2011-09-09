//#SRCHEADER

#include "LocoSystem.h"
#include "LocoProcess.h"

namespace loco {
    QVariant process() const {
        if( GetContext() == 0 ) {
            error( "NULL Context" );
            return QVariant();
        }
        Process* p = new Process;
        return GetContext()->AddObjToJSContext( f );
    }
}
