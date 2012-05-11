////////////////////////////////////////////////////////////////////////////////
//Copyright (c) 2012, Ugo Varetto
//All rights reserved.
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions are met:
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder nor the
//      names of its contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//DISCLAIMED. IN NO EVENT SHALL UGO VARETTO BE LIABLE FOR ANY
//DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////


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
