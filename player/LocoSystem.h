#pragma once
//#SRCHEADER

#include <cstdlib> //for dupenv (win), putenv, getenv

#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVariantMap>
#include <QProcess>
#include <QProcessEnvironment>
#include <QMutex>
#include <QWaitCondition>

#include "LocoObject.h"


namespace loco {

class System : public Object {
    Q_OBJECT

public:
   System() : Object( 0, "LocoSystem", "Loco/System/System" ) {}

public slots:

    bool putEnv( const QString& var, const QString& val ) {
        return ::putenv( const_cast<char*>(( var + "=" + val ).toAscii().constData()) ) == 0;
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
        #if defined( Q_OS_AIX ) || defined( Q_OS_BSD4 ) || defined( Q_OS_FREEBSD ) || \
            defined( Q_OS_SOLARIS ) || defined( Q_OS_LINUX ) || defined( Q_OS_UNIX )  
            return "UNIX";
        #elif defined( Q_OS_WIN )
            return "WINDOWS";
        #elif defined( Q_OS_MAC )
            return "MAC"
        #elif defined( Q_OS_SYMBIAN ) 
            return "SYMBIAN"
        #else
            return "";
        #endif
    }

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
      sleepWCond_.wait( &sleepMutex_, ms );   // two seconds
    }
private:
	mutable QMutex sleepMutex_;
	mutable QWaitCondition sleepWCond_;
};

}
