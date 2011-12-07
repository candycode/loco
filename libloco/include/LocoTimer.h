#pragma once
//#SRCHEADER

#include <QObject>

#include "LocoObject.h"

#ifdef _WIN32
#include <windows.h>
#include <WinNT.h>
#else
#include <sys/time.h>
#endif

namespace loco {

/// Computes elapsed time in milliseconds returned as a double precision floating point number.
/// Works on Linux, Windows and Mac OS.
class Timer : public Object {
    Q_OBJECT
    Q_PROPERTY( double elapsed READ ElapsedTime )
public:
    ///Default constructor.
    Timer();
    ///Start timer.
    void Start();
    ///Stop timer.
    ///\return elapsed time in milliseconds since call to Start()
    double Stop();
    ///Computes elapsed time in milliseconds between a Start() and Stop() calls
    ///\return time elapsed between subsequent Start() and Stop() function calls
    double ElapsedTime() const;
    ///Returns the time elapsed from the call to the Start() function;
    ///does not stop the timer.
    ///\return time in milliseconds elapsed from previous call to Start()
    double DTime() const; 
public slots:
    void start() { Start(); }
    double stop() { return Stop(); }
private:
    ///Reset timer.
    void Reset();
#ifdef WIN32
    LARGE_INTEGER freq_;
    LARGE_INTEGER tstart_;
    LARGE_INTEGER tend_;
    mutable LARGE_INTEGER tendTmp_;
#else
    timeval tstart_;
    timeval tend_;
    mutable timeval tendTmp_;
#endif
private:
    ///Do not allow construction from other instance.
    Timer( const Timer& );
    ///Forbid assignment.
    Timer operator=( const Timer& );
};


#ifdef WIN32

inline Timer::Timer() :  Object( 0, "LocoTimer", "/Loco/System/Timer" ) {
    ::QueryPerformanceFrequency( &freq_ );
    tstart_.QuadPart = 0;
    tend_.QuadPart = 0;
}

inline void Timer::Start() {
    ::QueryPerformanceCounter( &tstart_ );
}

inline double Timer::Stop()
    ::QueryPerformanceCounter( &tend_ );
    return DTime();
}

inline double Timer::ElapsedTime() const {
    ::QueryPerformanceCounter( &tendTmp_ );
    return 1000. * ( ( double ) tendTmp_.QuadPart -
           ( double ) tstart_.QuadPart) /( ( double ) freq_.QuadPart );
}

inline double Timer::DTime() const {
    return 1000. * ( ( double ) tend_.QuadPart -
           ( double ) tstart_.QuadPart) /( ( double ) freq_.QuadPart );
}

inline void Timer::Reset() {
    tstart_.QuadPart = 0;
    tend_.QuadPart = 0;
}

#else
inline Timer::Timer() : Object( 0, "LocoTimer", "/Loco/System/Timer" ) {
    Reset();
}

inline void Timer::Start() {
    ::gettimeofday( &tstart_, 0  );
}

inline double Timer::Stop() {
    ::gettimeofday( &tend_, 0 );
    return DTime();
}

inline double Timer::ElapsedTime() const {
    ::gettimeofday( &tendTmp_, 0 );
    const double t1 = ( double ) tstart_.tv_sec + ( double ) tstart_.tv_usec / 1E6;
    const double t2 = ( double ) tendTmp_.tv_sec   + ( double ) tendTmp_.tv_usec / 1E6;
    return 1000. * ( t2 - t1 );
}

inline double Timer::DTime() const {
    const double t1 = ( double ) tstart_.tv_sec + ( double ) tstart_.tv_usec / 1E6;
    const double t2 = ( double ) tend_.tv_sec   + ( double ) tend_.tv_usec / 1E6;
    return 1000. * ( t2 - t1 );
}

inline void Timer::Reset() {
    tstart_.tv_sec = 0;
    tstart_.tv_usec = 0;
    tend_.tv_sec = 0;
    tend_.tv_usec = 0;

}
#endif

} 

