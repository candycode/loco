#pragma once
//#SRCHEADER

#include <QObject>
#include <QString>
#include <iostream>

namespace loco {

class Console : public QObject {
    Q_OBJECT
public slots:
	void print(const QString& s) { std::cout << s.toAscii().constData(); }
	void println( const QString& s ) { print( s ); std::cout << std::endl; }
	void printerr( const QString& s ) { std::cerr << s.toAscii().constData(); }
	void printerrln( const QString& s { printerr( s ); std::cerr << std::endl; }
	QString read() const { std::string s; std::cin >> s; return QString( s.c_str() ); }
	bool end() const { return std::cin.eof(); }
	void destroy() { delete this; }
	void flush() { std::cout.flush(); std::cerr.flush(); }
};

}