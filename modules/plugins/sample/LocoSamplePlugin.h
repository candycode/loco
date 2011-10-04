#pragma once
//#SRCHEADER

#include <iostream>

#include <QObject>
#include <QtPlugin>

#include <LocoObject.h>

namespace loco {
class SamplePlugin : public Object {
	Q_OBJECT
	Q_INTERFACES( loco::Object )
public:
    SamplePlugin() : Object( 0, "SamplePlugin" ) {}
public slots:
	void hello() const { std::cout << "Hello from a loco Plugin :o" << std::endl; }
};

}

Q_EXPORT_PLUGIN2( loco_SamplePlugin, loco::SamplePlugin )
