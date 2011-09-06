#pragma once
//#SRCHEADER


class QString;

namespace loco {

class Object;
class Context;

struct Factory {
	
	virtual Object* Create( const QString& ) = 0;

    virtual void Destroy( Object* ) = 0;

    virtual void SetContext( Context* ) = 0;

	virtual ~Factory() = 0 {}

};

}


