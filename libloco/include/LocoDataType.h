#pragma once
//#SRCHEADER

#include "LocoObject.h"
#include <QVariant>

namespace loco {

class DataType : public Object {
    Q_OBJECT
	Q_PROPERTY( QVariant data READ data WRITE data )
public:
    DataType( const QVariant& data = QVariant() )
        : Object( 0, "LocoDataType", "/loco/data" ), data_( data ) {}
    const QVariant& data() const { return data_; }
    void data( const QVariant& d ) { data_ = d; }
private:
    QVariant data_;
};

}
