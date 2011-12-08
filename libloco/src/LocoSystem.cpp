//#SRCHEADER

#include "LocoSystem.h"
#include "LocoProcess.h"
#include "LocoEventLoop.h"
#include "LocoContext.h"
#include "LocoTimer.h"

namespace loco {
    QVariant System::process() const {
        if( GetContext() == 0 ) {
            error( "NULL Context" );
            return QVariant();
        }
        Process* p = new Process;
        return GetContext()->AddObjToJSContext( p );
    }

	QVariant System::eventLoop() const {
        if( GetContext() == 0 ) {
            error( "NULL Context" );
            return QVariant();
        }
        EventLoop* el = new EventLoop;
        return GetContext()->AddObjToJSContext( el );
	}

	QVariant System::timer() const {
	    if( GetContext() == 0 ) {
		    error( "NULL Context" );
		    return QVariant();
		}
		Timer* t = new Timer;
		return GetContext()->AddObjToJSContext( t );
	}
}
