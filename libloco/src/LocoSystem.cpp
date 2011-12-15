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
	QVariantMap System::SysInfo() const {
		QVariantMap info;
#if defined( Q_OS_LINUX ) || defined( Q_OS_MAC )
        info[ "numCpu" ] = int( sysconf( _SC_NPROCESSORS_ONLN ) );
        info[ "pageSize" ] = int( sysconf( _SC_PAGE_SIZE ) );
#elif defined( Q_OS_WIN )
        SYSTEM_INFO sysinfo;
        GetNativeSystemInfo( &sysinfo );
        info[ "numCpu" ] = int( sysinfo.dwNumberOfProcessors );
        info[ "pageSize" ] = int( sysinfo.dwPageSize );
#endif
        return info;
	}
}
