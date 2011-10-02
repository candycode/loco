#pragma once
//#SRCHEADER

#include <QObject>
#include <QString>
#include <iostream>

namespace loco {

class Stdin : public QObject {
    Q_OBJECT
public:
	//Stdout() { printf("Stdout::Stdout\n" ); }
public slots:
	
	QString read() { std::string s; std::cin >> s; return QString( s.c_str() ); }
};
}