#pragma once
#include "LocoArray.h"

// cannot create a proper macro, need to separate QT macros from rest
// to avoid 'undefined reference to `vtable for loco::...' link-time error
namespace loco {
class UIntArray : public Object {
	Q_OBJECT
	Q_PROPERTY( quint64 length READ GetLength )
    Q_PROPERTY( QString valueType READ GetValueType )
    Q_PROPERTY( QVariantList data READ GetData WRITE SetData )
private:
    unsigned ToUInt( const QVariant& v ) { return v.toUInt(); }
LOCO_DECLARE_ARRAY( UIntArray, unsigned, LocoUIntArray, ToUInt )
}


