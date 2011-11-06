#pragma once
//#SRCHEADER

#include <QString>
#include <QFile>

#include <iostream>

#include "LocoObject.h"
#include "LocoFileAccessManager.h"


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
    bool open( const QStringList& openMode );
    void close() {
        if( !file_.isOpen() ) {
            error( "File close requested on non-open file" );
        } else file_.close();
    }
    QString errMsg() { return file_.errorString(); }
    QByteArray readBytes( qint64 maxSize ) { return file_.read( maxSize ); }
    QString read( qint64 maxSize ) { return readBytes( maxSize ); }
    QString readLine( qint64 maxSize = 0 ) { return file_.readLine( maxSize ); }
    char getChar() {  file_.getChar( &charBuf_ ); return charBuf_; }
    void ungetChar( char c ) { file_.ungetChar( c ); }
    QString fileName() { return file_.fileName(); }
    qint64 size() { return file_.size(); }
    bool resize( qint64 ns ) { return file_.resize( ns ); }
    qint64 write( const char* data, qint64 maxSize = -1 ) { return file_.write( data, maxSize ); }
	qint64 write( const QString& data, qint64 maxSize = -1 ) { return file_.write( data.toAscii().constData(), maxSize ); }
    qint64 writeBytes( QByteArray data ) { return file_.write( data ); }
    bool reset() { return file_.reset(); }
    bool seek( qint64 pos ) { return file_.seek( pos ); }
    qint64 pos() { return file_.pos(); }
    QString readAll() { return file_.readAll(); }
    QByteArray readAllBytes() { return file_.readAll(); }  
    bool flush() { return file_.flush(); }

private:
    
    static QIODevice::OpenMode MapOpenMode( const QStringList& mode );
       
private:
    QFile file_;
    char charBuf_;
};

}
