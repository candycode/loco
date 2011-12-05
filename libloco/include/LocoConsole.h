#pragma once
//#SRCHEADER

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
	QString read() const { std::string s; std::cin >> s; return QString( s.c_str() ); }
	QString readLine() const { std::string s; std::getline( std::cin, s ); return QString( s.c_str() ); }
	int getc() const { return std::cin.get(); }
	void ungetc() const { std::cin.unget(); }
	bool end() const { return std::cin.eof(); }
	void flush()  const { std::cout.flush(); std::cerr.flush(); }
};

}
