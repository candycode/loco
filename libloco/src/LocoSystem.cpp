//#SRCHEADER

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
#   include <unistd.h>
#elif defined(Q_OS_MACX)
#   include <mach/mach.h>
#   include <mach/machine.h>
#endif

#include "LocoSystem.h"
#include "LocoProcess.h"
#include "LocoEventLoop.h"
#include "LocoContext.h"

namespace loco {
namespace {
int GetCpuCount() {
    int cpuCount = 1;
#if defined(Q_OS_WIN)
    {
		//SYSTEM_INFO si;
	    //GetSystemInfo(&si);
	    //cpuCount = si.dwNumberOfProcessors;
		cpuCount = 0;
    }
#elif defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
    cpuCount = sysconf(_SC_NPROCESSORS_ONLN);
#elif defined(Q_OS_MACX)
   kern_return_t		kr;
   struct host_basic_info	hostinfo;
   unsigned int			count;

   count = HOST_BASIC_INFO_COUNT;
   kr = host_info(mach_host_self(), HOST_BASIC_INFO, (host_info_t)&hostinfo, &count);
   if(kr == KERN_SUCCESS) {
	cpuCount = hostinfo.avail_cpus;
//	totalMemory = hostinfo.memory_size; 			 //in bytes
   }

#endif

    if( cpuCount < 1 )
	cpuCount = 1;

    return cpuCount;
}
}

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
	int System::cpuCount() const {
		return GetCpuCount();
	}

}
