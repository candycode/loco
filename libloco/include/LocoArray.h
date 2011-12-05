#pragma once
//#SRCHEADER

#include <vector>
#include <algorithm>

#include <QObject>
#include <QString>
#include <QList>

//cannot use templates (issues with moc/Qt)

namespace loco {

#ifdef LOCO_CREATE_ARRAY
#error "LOCO_CREATE_ARRAY already #defined"
#endif

#define LOCO_CREATE_ARRAY( ClassName, DataType, LocoClassName ) \
class ClassName : public Object { \
	Q_OBJECT \
	Q_PROPERTY( quint64 length READ length ) \
    Q_PROPERTY( QString valueType READ valueType ) \
public: \
	ClassName() : Object( 0, LocoClassName, "/Loco/Data/Array" ) {} \
    ClassName( const ClassName & other, quint64 start, quint64 length ) : \
        data_( other.data_.begin() + start, other.data_.begin() + start + length ) {} \
    ClassName( const ClassName & other ) : data_( other.data_ ) {}
    DataType & operator[]( quint64 i ) { return data_[ i ]; } \
    const DataType & operator[]( quint64 i ) const { return data_[ i ]; } \
    ClassName * Clone() const { return new ClassName ( *this ); }
    void Insert( quint64 start, const ClassName & array  ) {
    	data_.resize( data_.size() + array.length() ); \
    	DataType * base = &data_[ 0 ]; \
    	memmove( base + start + array.length(), base + start, array.length() * sizeof( DataType ) ); \
    	memcpy(  base + start, &array_[ 0 ], array.length() * sizeof( DataType ) ); \
    } \
    void Set( quint64 start, const ClassName & array ) { \
        data_.resize( start + array.length ); \
        std::copy( data_.begin() + start, array.data_.begin(), array.data_.end() ); \
    } \
public slots: \
    void* rawData() { return &data_[ 0 ]; } \ //for calling e.g. C functions through JS
    int valueByteSize() const { return sizeof( DataType ); } \
    DataType at( quint64 idx ) { return data_[ idx ]; } \
    QList< DataType > at( quint64 start, quint64 size ) { \
        QList< DataType > sa; \
        sa.reserve( size ); \
        typedef std::vector< DataType > V; \
        for( V::const_iterator i = data_.begin(); \
        	 i != data_.end(); ++i ) { \
            sa.push_back( *i ); \
        } \
        return sa; \
    } \
    QString valueType() const { return #Type } \
    void set( quint64 idx, DataType v ) { data_[ idx ] = v; } \
    void set( quint64 idx, QObject* arrayObj ) { \
    	if( !qobject_cast< ClassName * >( arrayObj ) ) { \
    	    error( "Not a LocoArray type" ); \
    	    return; \
    	} \
    	ClassName * array = qobject_cast< ClassName * >( arrayObj ); \
    	Set( idx, *array );
    } \
    void set( quint64 start, const QList< Type >& data ) { \
    	data_.resize( start + data.length() ); \
        std::copy( data_.begin() + start, data.begin(), data.end() ); \
    } \
    quint64 length() const { return data_.size(); } \
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
    void insert( quint64 start, const QList< Type >& data ) { \
        ClassName array; \
        array.set( data ); \
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
        data_.resize( start ); \
        return GetContext()->AddObjToJSContext( v ); \
    } \
private: \
    std::vector< DataType > data_; \
};

}

