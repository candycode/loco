//#SRCHEADER
#include "LocoObject.h"
#include "LocoContext.h"

namespace loco {
	QAtomicInt Object::instanceCount_;
	QString Object::objNamePrefix_ = "loco_";
	QString Object::objNameSuffix_ = "__";

	void Object::OnDestroy( QObject* obj ) {
        //set the pointer to this object in context to NULL
        if( context_ != 0 ) context_->RemoveJSCtxObject( this );
    }
}