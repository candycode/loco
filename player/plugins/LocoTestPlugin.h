#pragma once
//#SRCHEADER


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
    void hello() const;
};
}
