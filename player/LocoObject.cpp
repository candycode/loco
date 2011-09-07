//#SRCHEADER
#include "LocoObject.h"

#include "LocoFactory.h"

namespace loco {
	QAtomicInt Object::instanceCount_;
	QString Object::objNamePrefix_ = "loco_";
	QString Object::objNameSuffix_ = "__";
}