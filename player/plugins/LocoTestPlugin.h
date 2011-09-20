#pragma once
//#SRCHEADER

#include <iostream>

#include <QObject>
#include <QtPlugin>

#include "../LocoObject.h"


namespace loco {
class TestPlugin : public Object {
	Q_OBJECT
	Q_INTERFACES( loco::Object )
public:
    TestPlugin() : Object( 0, "TestPlugin" ) {}
public slots:
	void hello() const { std::cout << "Hello from a loco Plugin :o" << std::endl; }
};
}

Q_EXPORT_PLUGIN2( loco_TestPlugin, loco::TestPlugin )
