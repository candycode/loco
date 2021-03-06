#pragma once
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


#include <cstdlib> //for dupenv (win), putenv, getenv
#include <ctime> //clock

#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVariantMap>
#include <QProcess>
#include <QProcessEnvironment>
#include <QMutex>
#include <QWaitCondition>
#include <QThread>

#include "LocoObject.h"

namespace loco {

class System : public Object {
    Q_OBJECT
    Q_PROPERTY( quint64 CLOCKS_PER_SECOND READ clocksPerSecond )
    Q_PROPERTY( QVariantMap sysInfo READ SysInfo )
public:
   System() : Object( 0, "LocoSystem", "Loco/System/System" ) {}
   QVariantMap SysInfo() const;
public slots:
    QVariant timer() const;
    bool putEnv( const QString& var, const QString& val ) {
#ifndef Q_OS_WIN
        return ::putenv( const_cast<char*>(( var + "=" + val ).toAscii().constData()) ) == 0;
#else
        return ::_putenv( ( var + "=" + val ).toAscii().constData() ) == 0;
#endif
    }
    
    QString getEnv( const QString& envVarName ) const {
#if !defined( Q_WS_WIN )
        return ::getenv( envVarName.toAscii().constData() );
#else
        char* p = 0;
        size_t len = 0;
        errno_t err = _dupenv_s( &p, &len, envVarName.toStdString().c_str() );
        if( err ) return "";
        QString ret( p );
        free( p );
        return ret;
#endif
    }   
    QString os() const {
        #if defined( Q_OS_LINUX )
            return "LINUX";
        #elif defined( Q_OS_MAC )
            return "MAC";
        #elif defined( Q_OS_AIX ) || defined( Q_OS_BSD4 ) || defined( Q_OS_FREEBSD ) || \
            defined( Q_OS_SOLARIS ) || defined( Q_OS_UNIX )
            return "UNIX";
        #elif defined( Q_OS_WIN )
            return "WINDOWS";
        #elif defined( Q_OS_SYMBIAN ) 
            return "SYMBIAN"
        #else
            return "";
        #endif
    }
    QString qtVersion() const { return qVersion(); }
    bool hasGUI() const {
        #if defined( Q_WS_WIN )
            return true;
        #elif defined( Q_WS_MAC )
            return true;
        #elif defined( Q_WS_X11 )
            return true;
        #elif defined( Q_WS_S60 )
            return true;
        #else
            return false;
        #endif
    }     
    QString run( const QString& program,
                 const QStringList& args = QStringList(),
                 const QVariantMap& env  = QVariantMap(),
                 int timeout = 10000 ) const {
        QProcessEnvironment pe;
        for( QVariantMap::const_iterator i = env.begin();
             i != env.end(); ++i ) {
            const QString& envVar = i.key();
            if( envVar.isEmpty() ) continue;
            const QVariant& v = i.value();
            if( v.isNull() || !v.isValid() /*|| v.toString().isEmtpy()*/ ) continue;
            pe.insert( i.key(), v.toString() );
        }
        QProcess p;
        p.setProcessEnvironment( pe );
        p.start( program, args );
        const bool wff = p.waitForFinished( timeout );
        if( !wff ) {
            if( p.error() == QProcess::FailedToStart     ) error( "QProcess: FailedToStart" );
            else if( p.error() == QProcess::Crashed      ) error( "QProcess: Crashed" );
            else if( p.error() == QProcess::Timedout     ) error( "QProcess: Timedout" );
            else if( p.error() == QProcess::WriteError   ) error( "QProcess: WriteError" );
            else if( p.error() == QProcess::ReadError    ) error( "QProcess: ReadError" );
            else if( p.error() == QProcess::UnknownError ) error( "QProcess: UnknownError" );
            else error( "Unknown QProcess::start error" );
            return "";
        } else {
            QString output = p.readAllStandardOutput();
            if( output.size() == 0 ) output = p.readAllStandardError();
            return output;
        }                          
    } 
    qint64 runDetached( const QString& program,
                        const QStringList& args,
                        const QString& workingDir = "./" ) {
            qint64 pid = -1;
            QProcess::startDetached( program, args, workingDir, &pid );
            return pid;

    }
    QVariantMap env() const {
        QProcessEnvironment pe = QProcessEnvironment::systemEnvironment();
        QStringList e = pe.toStringList();
        QVariantMap vm;
        for( QStringList::const_iterator i = e.begin(); i != e.end(); ++i ) {
            QStringList kv = i->split( "=" );
            vm[ *kv.begin() ] = *( ++kv.begin() );
        }
        return vm;          
    }
    QVariant process() const;
    QVariant eventLoop() const;
    void sleep( int ms ) const {
      sleepMutex_.lock();
      sleepWCond_.wait( &sleepMutex_, ms );
    }
    int cpuCount() const { return QThread::idealThreadCount(); }
    quint64 threadId() const { return quint64( QThread::currentThreadId() ); }
    quint64 clock() const { return quint64( ::clock() ); }
    quint64 clocksPerSecond() const { return CLOCKS_PER_SEC; }
private:
    mutable QMutex sleepMutex_;
    mutable QWaitCondition sleepWCond_;
};

}
