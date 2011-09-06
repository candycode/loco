#pragma once
//#SRCHEADER

#include "LocoObject.h"

class QString;

namespace loco {

class Object;
class Context;

class Factory : public Object {
    Q_OBJECT	
public:
	virtual Object* Create( const QString& ) = 0;

    virtual void Destroy( Object* ) = 0;

    virtual void Destroy() = 0;

    virtual void SetContext( Context* ) = 0;

};

}


