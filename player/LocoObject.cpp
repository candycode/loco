//#SRCHEADER
#include "LocoObject.h"
namespace loco {
	QAtomicInt Object::instanceCount_;
	QAtomicInt Object::objNamePrefix_ = "loco_";
	QAtomicInt Object::objNameSuffix  = "__";
}