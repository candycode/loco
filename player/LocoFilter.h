#pragma once
//#SRCHEADER

#include "EWL.h"

namespace loco {

struct Filter : EWL {
    virtual QString Filter( const QString& ) = 0;
	virtual ~Filter() = 0 {}
};

}
