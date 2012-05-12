#pragma once
//#SRCHEADER

#include <vector>
#include <algorithm>

#include <QObject>
#include <QString>
#include <QVariantList>

#include "LocoObject.h"
#include "LocoContext.h"

#include "DynamicAlignedAllocator.h"

//cannot use templates (issues with moc/Qt)
//cannot declare slots in a macro because moc does not expand regular macros before the 'slot' macro
// i.e. the following results into an "Error: Not a signal or slot declaration"
//public slots:
// LOCO_DECLARE_SLOTS_MACRO

#ifdef LOCO_DECLARE_ARRAY_METHODS
#error "LOCO_DECLARE_ARRAY_METHODS already #defined"
#endif

#define LOCO_DECLARE_ARRAY_METHODS \
public: \
    typedef std::vector< ValueType, DynamicAlignedAllocator< ValueType > > Vector; \
    typedef Vector::allocator_type Allocator; \
    ClassName ( const Vector::allocator_type& a = Vector::allocator_type() ) : \
        Object( 0, LocoClassName , "/Loco/Data/Array" ), \
        data_( a ) {} \
    ClassName ( const ClassName & other, quint64 start, quint64 length ) : \
        data_( other.data_.begin() + start, other.data_.begin() + start + length ) {} \
    ClassName ( const ClassName & other ) : data_( other.data_ ) {} \
    QVariantList GetData() const { \
        QVariantList data; \
        data.reserve( QVariantList::size_type( data_.size() ) ); \
        Vector::const_iterator i = data_.begin(); \
        for( ; i != data_.end(); ++i ) data.push_back( *i ); \
        return data; \
    } \
    void SetData( const QVariantList& data ) { \
        data_.clear(); \
        for( QVariantList::const_iterator i = data.begin(); \
             i != data.end(); ++i ) { \
            data_.push_back( ToType( *i ) ); \
        } \
    } \
    quint64 GetLength() const { return data_.size(); } \
    ValueType & operator[]( quint64 i ) { return data_[ i ]; } \
    const ValueType & operator[]( quint64 i ) const { return data_[ i ]; } \
    ClassName * Clone() const { return new ClassName ( *this ); } \
    void Insert( quint64 start, const ClassName & array  ) { \
        data_.resize( data_.size() + array.GetLength() ); \
        ValueType * base = &data_[ 0 ]; \
        memmove( base + start + array.GetLength(), base + start, array.GetLength() * sizeof( ValueType ) ); \
        memcpy(  base + start, &array[ 0 ], array.GetLength() * sizeof( ValueType ) ); \
    } \
    void Set( quint64 start, const ClassName & array ) { \
        data_.resize( start + array.data_.size() ); \
        std::copy( array.data_.begin(), array.data_.end(), data_.begin() + start ); \
    } \
    QString GetValueType() const { return ValueTypeName ; } \
    quint64 GetAlignment() const { return data_.get_allocator().alignment(); } \
    quint64 AddressOf( quint64 elementIdx ) const { return reinterpret_cast< quint64 >( &data_[ elementIdx ] ); } \
    quint64 Address() const { return reinterpret_cast< quint64 >( &data_[ 0 ] ); }


