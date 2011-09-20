//#SRCHEADER

#include <iostream>

#include <QtPlugin>

#include "LocoTestPlugin.h"

namespace loco {

void TestPlugin::hello() const {
    std::cout << "Hello from a loco Plugin :o" << std::endl;

}
}

Q_EXPORT_PLUGIN2( loco_TestPlugin, loco::TestPlugin )