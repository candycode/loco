#pragma once
//#SRCHEADER

#include "LocoObject.h"
#include <QVariantMap>

namespace loco {

class DataType : public Object {
    Q_OBJECT
	Q_PROPERTY( QVariantMap data READ data WRITE data )
public:
    DataType( const QVariantMap& data = QVariantMap() )
        : Object( 0, "LocoDataType", "/loco/data" ), data_( data ) {}
    const QVariantMap& data() const { return data_; }
    void data( const QVariantMap& d ) { data_ = d; }
private:
    QVariantMap data_;
};

}
