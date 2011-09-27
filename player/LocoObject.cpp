//#SRCHEADER
#include "LocoObject.h"
#include "LocoContext.h"

namespace loco {
	QAtomicInt Object::instanceCount_;
	QString Object::objNamePrefix_ = "loco_";
	QString Object::objNameSuffix_ = "__";
}