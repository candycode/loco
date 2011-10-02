#pragma once
//#SRCHEADER

#include <QObject>
#include <QString>
#include <iostream>

namespace loco {

class Stdout : public QObject {
    Q_OBJECT
public:
	//Stdout() { printf("Stdout::Stdout\n" ); }
public slots:
	
	void print(const QString& s) { std::cout << s.toAscii().constData(); }
	void println( const QString& s ) { print(s); std::cout << std::endl; }
	void destroy() { delete this; }
	void flush() { std::cout.flush(); }
};
}