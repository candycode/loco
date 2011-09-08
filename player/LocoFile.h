#pragma once
//#SRCHEADER

#include <QString>
#include <QFile>

#include <iostream>

#include "LocoObject.h"

namespace loco {

class File : public Object {
    Q_OBJECT

public:
   File() : Object( 0, "LocoFile", "Loco/IO/File" ) {}

public slots:

	bool open( const QString& filePath, const QStringList& mode ) {
        setFileName( filePath );
        return open( mode );          
    }

    void setFileName( const QString& n ) { 
        if( file_.fileName() != n ) {
            if( file_.isOpen() ) file_.close();
        }
        file_.setFileName( n );
    }
   
    bool open( const QStringList& openMode ) {
        if( file_.fileName().isEmpty() ) {
            Error( "Empty file name" );
            return false; 
        } else if( !file_.exists() ) { 
            Error( "File " + file_.fileName() + " doesn't exist" );
            return false;
        } else if( !file_.open( MapOpenMode( openMode ) ) ) {
            Error( "File " + file_.fileName() + " open failed" );
            return false;
        }
        else return true;             
    }

    void close() {
        if( !file_.isOpen() ) {
            Error( "File close requested on non-open file" );
        } else file_.close();
    }

    QString errMsg() { return file_.errorString(); }

    QByteArray readBytes( qint64 maxSize ) { return file_.read( maxSize ); }

    QString read( qint64 maxSize ) { return readBytes( maxSize ); }

    QString readLine( qint64 maxSize = 0 ) { return file_.readLine( maxSize ); }

    char getChar() { char c; file_.getChar( &c ); return c; }

    void ungetChar( char c ) { file_.ungetChar( c ); }

    QString fileName() { return file_.fileName(); }

    qint64 size() { return file_.size(); }

    bool resize( qint64 ns ) { return file_.resize( ns ); }

    qint64 write( const char* data, qint64 maxSize ) { return file_.write( data, maxSize ); }

    qint64 write( QByteArray data ) { return file_.write( data ); }

    bool reset() { return file_.reset(); }

    bool seek( qint64 pos ) { return file_.seek( pos ); }

    qint64 pos() { return file_.pos(); }

    QString readAll() { return file_.readAll(); }

    QByteArray readAllBytes() { return file_.readAll(); }
  
    bool flush() { return file_.flush(); }

private:
    
    QIODevice::OpenMode MapOpenMode( const QStringList& mode ) {
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
    

     
       
private:
    QFile file_;
};

}