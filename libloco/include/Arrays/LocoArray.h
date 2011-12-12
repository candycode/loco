#pragma once
//#SRCHEADER

#include <vector>
#include <algorithm>

#include <QObject>
#include <QString>
#include <QVariantList>

#include "LocoObject.h"
#include "LocoContext.h"

//cannot use templates (issues with moc/Qt)

#ifdef LOCO_DECLARE_ARRAY
#error "LOCO_DECLARE_ARRAY already #defined"
#endif

// e.g.
// namespace loco {
//class Float64Array : public Object {
//	Q_OBJECT
//	Q_PROPERTY( quint64 length READ GetLength )
//  Q_PROPERTY( QString valueType READ GetValueType )
//  Q_PROPERTY( QVariantList data READ GetData WRITE SetData )
//LOCO_DECLARE_ARRAY( Float64Array, double, LocoFloat64Array, toDouble )
//}
#define LOCO_DECLARE_ARRAY( ClassName, DataType, LocoClassName, ToType ) \
public: \
	ClassName () : Object( 0, #LocoClassName , "/Loco/Data/Array" ) {} \
    ClassName ( const ClassName & other, quint64 start, quint64 length ) : \
        data_( other.data_.begin() + start, other.data_.begin() + start + length ) {} \
    ClassName ( const ClassName & other ) : data_( other.data_ ) {} \
    QVariantList GetData() const { \
        QVariantList data; \
        data.reserve( data_.size() ); \
        std::vector< DataType >::const_iterator i = data_.begin(); \
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
    DataType & operator[]( quint64 i ) { return data_[ i ]; } \
    const DataType & operator[]( quint64 i ) const { return data_[ i ]; } \
    ClassName * Clone() const { return new ClassName ( *this ); } \
    void Insert( quint64 start, const ClassName & array  ) { \
    	data_.resize( data_.size() + array.GetLength() ); \
    	DataType * base = &data_[ 0 ]; \
    	memmove( base + start + array.GetLength(), base + start, array.GetLength() * sizeof( DataType ) ); \
    	memcpy(  base + start, &array[ 0 ], array.GetLength() * sizeof( DataType ) ); \
    } \
    void Set( quint64 start, const ClassName & array ) { \
        data_.resize( start + array.data_.size() ); \
        std::copy( array.data_.begin(), array.data_.end(), data_.begin() + start ); \
    } \
    QString GetValueType() const { return #DataType ; } \
public slots: \
    int valueByteSize() const { return sizeof( DataType ); } \
    DataType at( quint64 idx ) { return data_[ idx ]; } \
    QVariantList at( quint64 start, quint64 size ) const { \
        QVariantList sa; \
        sa.reserve( size ); \
        typedef std::vector< DataType > V; \
        for( V::const_iterator i = data_.begin(); \
        	 i != data_.end(); ++i ) { \
            sa.push_back( *i ); \
        } \
        return sa; \
    } \
    void set( quint64 idx, DataType v ) { data_[ idx ] = v; } \
    void set( quint64 idx, QObject* arrayObj ) { \
    	if( !qobject_cast< ClassName * >( arrayObj ) ) { \
    	    error( "Not a LocoArray type" ); \
    	    return; \
    	} \
    	ClassName * array = qobject_cast< ClassName * >( arrayObj ); \
    	Set( idx, *array ); \
    } \
    void set( quint64 start, const QVariantList& data ) { \
    	data_.resize( start + data.length() ); \
    	std::vector< DataType >::iterator out = data_.begin() + start; \
        for( QVariantList::const_iterator i = data.begin(); \
             i != data.end(); ++i, ++out ) *out = ToType( *i ); \
    } \
    void resize( quint64 sz ) { data_.resize( sz ); } \
    void clear() { data_.clear(); } \
    void push( DataType v ) { data_.push_back( v ); } \
    void insert( quint64 start, QObject* arrayObj ) { \
    	if( !dynamic_cast< ClassName * >( arrayObj ) ) { \
    	    error( "Not a LocoArray type" ); \
    	    return; \
    	} \
    	ClassName * array = qobject_cast< ClassName * >( arrayObj ); \
    	Insert( start, *array ); \
    } \
    void insert( quint64 start, const QVariantList& data ) { \
        ClassName array; \
        array.SetData( data ); \
        Insert( start, array ); \
    } \
    void reserve( quint64 sz ) { data_.reserve( sz ); } \
    void remove( quint64 start, quint64 length ) { \
        data_.erase( data_.begin() + start, data_.begin() + start + length ); \
    } \
    QVariant clone() const { \
        ClassName * v = new ClassName ( *this ); \
        return GetContext()->AddObjToJSContext( v ); \
    } \
    QVariant extract( quint64 start, quint64 length ) { \
        ClassName* v = new ClassName ( *this, start, length ); \
        remove( start, length ); \
        return GetContext()->AddObjToJSContext( v ); \
    } \
private: \
    std::vector< DataType > data_; \
};

