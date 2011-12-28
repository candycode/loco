#pragma once

#include <iostream>

#include <QObject>
#include <QString>

class MyObject : public QObject {
	Q_OBJECT
public slots:
	void method( const QString& msg ) {
		std::cout << msg.toStdString() << std::endl;
	}
	void emitSignal() { emit aSignal(); }
	void aSlot() { std::cout << "aSlot() called" << std::endl; }
signals:
	void aSignal();
};