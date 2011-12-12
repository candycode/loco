#pragma once
#include "LocoArray.h"

// cannot create a proper macro, need to separate QT macros from rest
// to avoid 'undefined reference to `vtable for loco::...' link-time error
namespace loco {
class ShortArray : public Object {
	Q_OBJECT
	Q_PROPERTY( quint64 length READ GetLength )
    Q_PROPERTY( QString valueType READ GetValueType )
    Q_PROPERTY( QVariantList data READ GetData WRITE SetData )
private:
    short ToShort( const QVariant& v ) { return short( v.toInt() ); }
LOCO_DECLARE_ARRAY( ShortArray, char, LocoShortArray, ToShort )
}


