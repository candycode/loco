#pragma once
//#SRCHEADER

#include <QEventLoop>

#include "LocoObject.h"

namespace loco {

class EventLoop : public Object {
    Q_OBJECT

public:
	EventLoop() : Object( 0, "LocoEventLoop", "Loco/System/EventLoop" )  {}

public slots:
	void exec() { el_.exec(); }
	void quit()  { el_.quit(); }
private:
    QEventLoop el_;

};

}

