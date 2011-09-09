#pragma once
//#SRCHEADER

#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVariantMap>
#include <QProcess>
#include <QProcessEnvironment>

#include "LocoObject.h"

namespace loco {

typedef QMap< QString, QString > ProcessEnv;


class Process : public Object {
    Q_OBJECT

public:
    Process() : Object( 0, "LocoProcess", "Loco/System/Process" ) {
        connect( &p_, SIGNAL( error( QProcess::ProcessError ) ),
                 this, OnProcessError( QProcess::ProcessError ) ) );
        connect( &p_, SIGNAL( finished( int, QProcess::ExitStatus ),
                 this, SLOT( OnProcessFinished( int, QProcess::ExitStatus ) ) );
        connect( &p_, SIGNAL( readyReadStandardError(),
                 this, SLOT( OnProcessReadStdErr() ) );
        connect( &p_, SIGNAL( readyReadStandardOutput(),
                 this, SLOT( OnProcessReadStdOut() ) );
        connect( &p_, SIGNAL( started(),
                 this, SLOT( OnProcessStarted() ) );
        connect( &p_, SIGNAL( stateChanged( QProcess::ProcessState ),
                 this, SLOT( OnProcessStateChanged( QProcess::ProcessState ) ) );  
    } 

public slots:
    void start( const QString& program,
                const QStringList& args,
                const QStringList& openmode,
                bool waitForStart = false ) {
        p_.start( program, args, MapOpenMode( openmode ) );
        if( waitForStarted ) p_.waitForStarted();
    }
    void kill() { p_.kill(); }
    void terminate() { p_.terminate(); }
    void close() { p_.close(); }
    bool end() const { p_.atEnd(); }
    qint64 bytesAvailable() const { return p_.bytesAvailable(); }
    qint64 bytesToWrite() const { return p_.bytesToWrite(); }
    virtual bool waitForBytesWritten( int msecs = 30000 ) { return p_.waitForBytesWritten( msecs ); }
    virtual bool waitForReadyRead( int msecs = 30000 ) { return p_.waitForBytesRead( msecs ); }
    void closeReadChannel( const QString& ch ) {
        p_.closeReadChannel( MapReadChannel( ch ) );
    }
    void closeWriteChannel() { p_.closeWriteChannel(); }
    int	exitCode () const { return p_.exitCode(); }
    QString	exitStatus () const {
        if( p_.exitStatus() == QProcess::NormalExit ) return "normal";
        else if( p_.exitStatus() == QProcess::CrashExit ) return "crash";
        else return QString("%1").arg( p_.exitStatus() );
    }
    QString	nativeArguments () const { return p_.nativeArguments(); }
    Q_PID pid () const { return p_.pid(); }
    QString	processChannelMode () const { 
        if( p_.processChannelMode() == QProcess::SeparateChannels )
            return "separate";
        else if( p_.processChannelMode() == QProcess::MergedChannels )
            return "merged";
        else if( p_.processChannelMode() == QProcess::ForwardedChannels )
            return "forwarded";
    }
    ProcessEnv processEnvironment () const {
        QStringList e = p_.processEnvironment();
        ProcessEnv penv;
        for( QStringList::const_iterator i = e.begin(); i != e.end(); ++i ) {
            QStringList v = i->split( "=" );
            if( ! *v.begin().isEmtpy() ) penv[ *v.begin() ] = *( ++v.begin() );
        }
        return penv;
    }
    QByteArray	readAllStandardError() { return p_.readAllStandardError(); }
    QByteArray	readAllStandardOutput() { return p_.readAllStandardError(); }
    QString readChannel () const {
        if( p_.readChannel() == QProcess::StandardOutput ) return "stdout";
        else if( p_.readChannel() == QProcess::StandardError ) return "stderr";
        else return QString("%1").arg( p_.readChannel() );
    }
    void setNativeArguments ( const QString& arguments ) { p_.setNativeArguments( arguments ); }
    void setProcessChannelMode( const QString& chm ) {
        p_.setProcessChannelMode( MapChannelMode( chm ) );
    }
    void setProcessEnvironment ( const ProcessEnv& e ) {
        QProcessEnvironment pe;
        for( ProcessEnv::const_itreator i = e.begin(); i != e.end(); ++i ) {
           pe.insert( e.key(), e.value() );
        }
    }
    void setReadChannel( const QString& ch ) { p_.readChannel( MapReadChannel( ch ); }
    void setStandardErrorFile( const QString& fileName, const QStringList& mode ) {
        p_.setStandardErrorFile( fileName, MapOpenMode( mode ) );
    }
    void setStandardInputFile ( const QString& fileName ) { p_.setStandardInputFile( fileName ); }
    void setStandardOutputFile( const QString& fileName, const QStringList& mode ) {
        p_.setStandardOutputFile( fileName, MapOpenMode( mode ) );
    }
    void setStandardOutputProcess ( const QVariantMap& dest ) {
        if( GetContext() == 0 ) {
            error( "NULL Context" );
            return; 
        }
        Object* obj = GetContext()->Find( dest[ "jsInstance" ] );
        if( obj == 0 ) {
            error( "Process object not found" );
            return;
        } 
        Process* p = qobject_cast< Process* >( obj );
        if( p == 0 ) {
            error( "Wrong object type: " + obj->type() );
            return; 
        }
        p_.setStandardOutputProcess( p );  
    }
    void setWorkingDirectory( const QString& dir ) { p_.setWorkingDirectory( dir ); }
//    QProcess::ProcessState state() const
    bool waitForFinished( int msecs = 30000 ) { return p_.waitForFinished( msecs ); }
    bool waitForStarted ( int msecs = 30000 ) { return p_.waitForStarted( msecs ); }
    QString	workingDirectory() const { return p_.workingDirectory(); }

private:
    static QIODevice::OpenMode MapOpenMode( const QStringList& mode ) {
        QIODevice::OpenMode om = 0;
        for( QStringList::const_iterator i = mode.begin(); i != mode.end(); ++i ) {
            const QString& m = *i;
            if( m == "r" ) om |= QIODevice::ReadOnly;
            else if( m == "w" ) om |= QIODevice::WriteOnly;
            else if( m == "a" ) om |= QIODevice::Append;
            else if( m == "truncate" ) om |= QIODevice::Truncate;
            else if( m == "text" ) om |= QIODevice::Text;
            else if( m == "unbuffered" ) om |= QIODevice::Unbuffered;
        }
        return om;   
    }
    static QProcess::ProcessChannel MapReadChannel( const QString& ch ) {
        QProcess::ProcessChannel m;
        if( ch == "stdout" ) m = QProcess::StandardOutput;
        else if( ch == "stderr" ) m = QProcess::StandardError;
        else error( "Unknown channel: " + ch );
        return m;
    }
    static QProcess::ProcessChannelMode MapChannelMode( const QString& chm ) {
        QProcessChannelMode pcm; 
        if( chm == "separate" ) pcm = QProcess::SeparateChannels;
        else if( chm == "merged" ) pcm = QProcess::MergedChannels
        else if( chm == "forwarded" ) pcm = QProcess::ForwardedChannels;
        else error( "Unknown channel mode" );
        return pcm;
    }   

private:
    QProcess p_

};

}

