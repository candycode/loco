#pragma once
//#SRCHEADER

#include "EWL.h"

#include <QString>

namespace loco {

class Filter : public EWL {
    Q_OBJECT
public:
    virtual QString Apply( const QString& ) const = 0;
};

}

Q_DECLARE_INTERFACE( loco::Filter, "Loco.Filter:1.0" )
