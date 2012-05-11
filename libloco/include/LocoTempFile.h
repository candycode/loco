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


#include <QString>
#include <QByteArray>
#include <QTemporaryFile>

#include <iostream>

#include "LocoObject.h"


namespace loco {

class TempFile : public Object {
    Q_OBJECT

public:
   TempFile( const QString& templateName ) :
	   Object( 0, "LocoTempFile", "Loco/IO/File" ), file_( templateName ) {}
public slots:
	bool open() {
        return file_.open();
    }
	QString fileName() const { return file_.fileName(); }
	void setAutoRemove( bool on ) { file_.setAutoRemove( on ); } //on by default
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
    QTemporaryFile file_;
    mutable char charBuf_;
};

}
