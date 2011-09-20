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
                 this, SLOT( OnProcessError( QProcess::ProcessError ) ) );
        connect( &p_, SIGNAL( finished( int, QProcess::ExitStatus ) ),
                 this, SLOT( OnProcessFinished( int, QProcess::ExitStatus ) ) );
        connect( &p_, SIGNAL( readyReadStandardError() ),
                 this, SLOT( OnProcessStdErrReady() ) );
        connect( &p_, SIGNAL( readyReadStandardOutput() ),
                 this, SLOT( OnProcessStdOutReady() ) );
        connect( &p_, SIGNAL( started() ),
                 this, SLOT( OnProcessStarted() ) );
        connect( &p_, SIGNAL( stateChanged( QProcess::ProcessState ) ),
                 this, SLOT( OnProcessStateChanged( QProcess::ProcessState ) ) );  
    } 

private slots:

    void OnProcessError( QProcess::ProcessError pe ) {
    }
    void OnProcessFinished( int ret, QProcess::ExitStatus es ) {
    }
    void OnProcessStdErrReady() {
    }
    void OnProcessStdOutReady() {    
    }
    void OnProcessStarted() {
    }
    void OnProcessStateChanged( QProcess::ProcessState ps ) {
    }



public slots:
    void start( const QString& program,
                const QStringList& args,
                const QStringList& openmode,
                bool waitForStart = false ) {
        p_.start( program, args, MapOpenMode( openmode ) );
        if( waitForStart ) p_.waitForStarted();
    }
    void kill() { p_.kill(); }
    void terminate() { p_.terminate(); }
    void close() { p_.close(); }
    bool end() const { return p_.atEnd(); }
    qint64 bytesAvailable() const { return p_.bytesAvailable(); }
    qint64 bytesToWrite() const { return p_.bytesToWrite(); }
    virtual bool waitForBytesWritten( int msecs = 30000 ) { return p_.waitForBytesWritten( msecs ); }
    virtual bool waitForReadyRead( int msecs = 30000 ) { return p_.waitForReadyRead( msecs ); }
    void closeReadChannel( const QString& ch ) {
        p_.closeReadChannel( MapReadChannel( ch ) );
    }
    void closeWriteChannel() { p_.closeWriteChannel(); }
    int	exitCode() const { return p_.exitCode(); }
    QString	exitStatus () const {
        if( p_.exitStatus() == QProcess::NormalExit ) return "normal";
        else if( p_.exitStatus() == QProcess::CrashExit ) return "crash";
        else return QString("%1").arg( p_.exitStatus() );
    }
    QString	nativeArguments() const { 
#if defined(Q_OS_WIN) || defined(Q_OS_SYMBIAN)         
        return p_.nativeArguments(); 
#else
        error( "nativeArguments not supprted on this system" );
        return "";
#endif
    }
    Q_PID pid () const { return p_.pid(); }
    QString	processChannelMode () const { 
        if( p_.processChannelMode() == QProcess::SeparateChannels )
            return "separate";
        else if( p_.processChannelMode() == QProcess::MergedChannels )
            return "merged";
        else if( p_.processChannelMode() == QProcess::ForwardedChannels )
            return "forwarded";
        else return ""; 
    }
    ProcessEnv processEnvironment () const {
        QStringList e = p_.processEnvironment().toStringList();
        ProcessEnv penv;
        for( QStringList::const_iterator i = e.begin(); i != e.end(); ++i ) {
            QStringList v = i->split( "=" );
            if( ! (*v.begin()).isEmpty() ) penv[ *v.begin() ] = *( ++v.begin() );
        }
        return penv;
    }
    QString	readAllStandardError() { return p_.readAllStandardError(); }
    QString	readAllStandardOutput() { return p_.readAllStandardOutput(); }
    QString readChannel () const {
        if( p_.readChannel() == QProcess::StandardOutput ) return "stdout";
        else if( p_.readChannel() == QProcess::StandardError ) return "stderr";
        else return QString("%1").arg( p_.readChannel() );
    }
    void setNativeArguments ( const QString& arguments ) { 
#if defined( Q_OS_WIN) || defined( Q_OS_SYMBIAN)
        p_.setNativeArguments( arguments );
#else
        error( "setNativeArguments not supported on this system" );
#endif
     }
    void setProcessChannelMode( const QString& chm ) {
        p_.setProcessChannelMode( MapChannelMode( chm ) );
    }
    void setProcessEnvironment ( const ProcessEnv& e ) {
        QProcessEnvironment pe;
        for( ProcessEnv::const_iterator i = e.begin(); i != e.end(); ++i ) {
           pe.insert( i.key(), i.value() );
        }
    }
    void setReadChannel( const QString& ch ) { p_.setReadChannel( MapReadChannel( ch ) ); }
    void setStandardErrorFile( const QString& fileName, const QStringList& mode ) {
        p_.setStandardErrorFile( fileName, MapOpenMode( mode ) );
    }
    void setStandardInputFile ( const QString& fileName ) { p_.setStandardInputFile( fileName ); }
    void setStandardOutputFile( const QString& fileName, const QStringList& mode ) {
        p_.setStandardOutputFile( fileName, MapOpenMode( mode ) );
    }
    void setStandardOutputProcess ( const QVariantMap& dest );
    void setWorkingDirectory( const QString& dir ) { p_.setWorkingDirectory( dir ); }
//    QProcess::ProcessState state() const
    bool waitForFinished( int msecs = 30000 ) { return p_.waitForFinished( msecs ); }
    bool waitForStarted ( int msecs = 30000 ) { return p_.waitForStarted( msecs ); }
    QString	workingDirectory() const { return p_.workingDirectory(); }
    QString read( qint64 maxSize ) { return p_.read( maxSize ); }
    QString readLine() { return p_.readLine(); } 
    qint64 write( const QString& d ) { return p_.write( d.toAscii().constData() ); }
    qint64 write( const QString& d, qint64 maxSize ) { return  p_.write( d.toAscii().constData(), maxSize ); }
    QString readAll() { return p_.readAll(); }  

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
    QProcess::ProcessChannel MapReadChannel( const QString& ch ) {
        QProcess::ProcessChannel m;
        if( ch == "stdout" ) m = QProcess::StandardOutput;
        else if( ch == "stderr" ) m = QProcess::StandardError;
        else error( "Unknown channel: " + ch );
        return m;
    }
    QProcess::ProcessChannelMode MapChannelMode( const QString& chm ) {
        QProcess::ProcessChannelMode pcm; 
        if( chm == "separate" ) pcm = QProcess::SeparateChannels;
        else if( chm == "merged" ) pcm = QProcess::MergedChannels;
        else if( chm == "forwarded" ) pcm = QProcess::ForwardedChannels;
        else error( "Unknown channel mode" );
        return pcm;
    }
   
private:
    
    QProcess& GetProcess() { return p_; }   

private:
    QProcess p_;

};

}

