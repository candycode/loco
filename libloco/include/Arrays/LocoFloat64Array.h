#pragma once
//#SRCHEADER

#include "LocoArray.h"
// cannot create a proper macro, need to separate QT macros from rest
// to avoid 'undefined reference to `vtable for loco::...' link-time error
namespace loco {

#define LocoClassName "Float64Array"
#define ToType ToDouble
#define ValueTypeName "double"
#define ClassName Float64Array

class Float64Array : public Object {
    Q_OBJECT
    Q_PROPERTY( quint64 length READ GetLength )
    Q_PROPERTY( QString valueType READ GetValueType )
    Q_PROPERTY( QVariantList data READ GetData WRITE SetData )
    Q_PROPERTY( quint64 address READ Address )
private:
    double ToDouble( const QVariant& v ) { return v.toDouble(); }
public:
    typedef double ValueType;
LOCO_DECLARE_ARRAY_METHODS
public slots:
    int valueByteSize() const { return sizeof( ValueType ); }
    QVariant at( quint64 idx ) { return data_[ idx ]; }
    QVariantList at( quint64 start, quint64 size ) const {
        QVariantList sa;
        sa.reserve( size );
        for( Vector::const_iterator i = data_.begin();
             i != data_.end(); ++i ) {
            sa.push_back( *i );
        }
        return sa;
    }
    void set( quint64 idx, const QVariant& v ) { data_[ idx ] = ToType( v ); }
    void set( quint64 idx, QObject* arrayObj ) {
        if( !qobject_cast< ClassName * >( arrayObj ) ) {
            error( "Not a LocoArray type" );
            return;
        }
        ClassName * array = qobject_cast< ClassName * >( arrayObj );
        Set( idx, *array );
    }
    void mset( quint64 start, const QVariantList& data ) {
        data_.resize( start + data.length() );
        Vector::iterator out = data_.begin() + start;
        for( QVariantList::const_iterator i = data.begin();
             i != data.end(); ++i, ++out ) *out = ToType( *i );
    }
    void resize( quint64 sz ) { data_.resize( sz ); }
    void clear() { data_.clear(); }
    void push( const QVariant& v ) { data_.push_back( ToType( v ) ); }
    void swap( QObject* arrayObj ) {
        if( !dynamic_cast< ClassName * >( arrayObj ) ) {
            error( "Not a LocoArray type" );
            return;
        }
        ClassName * array = qobject_cast< ClassName * >( arrayObj );
        array->data_.swap( data_ );
    }
    void insert( quint64 start, QObject* arrayObj ) {
        if( !dynamic_cast< ClassName * >( arrayObj ) ) {
            error( "Not a LocoArray type" );
            return;
        }
        ClassName * array = qobject_cast< ClassName * >( arrayObj );
        Insert( start, *array );
    }
    void insert( quint64 start, const QVariantList& data ) {
        ClassName array;
        array.SetData( data );
        Insert( start, array );
    }
    void reserve( quint64 sz ) { data_.reserve( sz ); }
    void remove( quint64 start, quint64 length ) {
        data_.erase( data_.begin() + start, data_.begin() + start + length );
    }
    QVariant clone() const {
        ClassName * v = new ClassName ( *this );
        return GetContext()->AddObjToJSContext( v );
    }
    QVariant extract( quint64 start, quint64 length ) {
        ClassName* v = new ClassName ( *this, start, length );
        remove( start, length );
        return GetContext()->AddObjToJSContext( v );
    }
    quint64 addressOf( quint64 idx ) const { return AddressOf( idx ); }
private:
    Vector data_;
};

#undef LocoClassName
#undef ToType
#undef ValueTypeName
#undef ClassName

}


