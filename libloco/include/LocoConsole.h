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


#include <QObject>
#include <QString>
#include <iostream>

#include "LocoObject.h"

namespace loco {

class Console : public Object {
    Q_OBJECT
public:
    Console() : Object( 0, "LocoConsole", "Loco/IO/Console" ) {}
public slots:
    void print( const QString& s) const { std::cout << s.toAscii().constData(); }
    void println( const QString& s ) const { print( s ); std::cout << std::endl; }
    void log( const QString& s ) const { println( s ); }
    void println() const { std::cout << std::endl; }
    void printerr( const QString& s ) const { std::cerr << s.toAscii().constData(); }
    void printerrln( const QString& s ) const { printerr( s ); std::cerr << std::endl; }
    void printerrln() const { std::cerr << std::endl; }
    void printr( const QString& s ) { std::cout << '\r' << s.toAscii().constData(); }
    QString read() const { std::string s; std::cin >> s; return QString( s.c_str() ); }
    QString readLine() const { std::string s; std::getline( std::cin, s ); return QString( s.c_str() ); }
    int getc() const { return std::cin.get(); }
    void ungetc() const { std::cin.unget(); }
    bool end() const { return std::cin.eof(); }
    void flush()  const { std::cout.flush(); std::cerr.flush(); }
    void slot() { }//std::cout << f.toFloat() << std::endl; }
    void emitSignal( float f ) { emit sig( f ); }
signals:
    void sig( float );
};



}
