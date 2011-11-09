//#SRCHEADER
#include <QVariant>

#include "LocoObject.h"
#include "LocoContext.h"

namespace loco {

QAtomicInt Object::instanceCount_;
QString Object::objNamePrefix_ = "loco_";
QString Object::objNameSuffix_ = "__";

QVariant Object::JSInstance() const {
	if( !GetContext() ) return QVariant();
	else return GetContext()->Eval( jsInstanceName() );
}

}
