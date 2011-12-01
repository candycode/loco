#pragma once
//#SRCHEADER

#include <vector>
#include <QObject>
#include <QString>
#include <QVector>

//cannot use templates (issues with moc/Qt)

namespace loco {

#ifdef LOCO_CREATE_ARRAY
#error "LOCO_CREATE_ARRAY already #defined"
#endif

#define LOCO_CREATE_ARRAY( ClassName, DataType, LocoClassName ) \
class ClassName : public Object { \
	Q_OBJECT \
	Q_PROPERTY( quint64 length READ length ) \
    Q_PROPERTY( QString type READ typeName ) \
public: \
	ClassName() : Object( 0, LocoClassName, "/Loco/Data/Array" ) {} \
    ClassName( const ClassName& other, quint64 start, quint64 length )
    : data_( other.begin() + start, other.begin() + start + length ) {} \
    Type & operator[]( quint64 i ) { return data_[ i ] }; \
    const Type & operator[]( quint64 i ) const { return data_[ i ]; } \
public slots: \
    char* rawData() { return reinterpret_cast< char* >( &data_[ 0 ] ); } \ //for calling e.g. C functions through JS
    Type at( quint64 idx ) { \
        return data_[ idx ]; \
    } \
    QString type() const { return #Type } \
    void set( quint64 idx, Type v ) { data_[ idx ] = v; } \
    void set( quint64 idx, QObject* arrayObj ) { \
    	if( !qobject_cast< ClassName* >( arrayObj ) ) { \
    	    error( "Not a LocoArray type" ); \
    	    return; \
    	} \
    	ClassName* array = qobject_cast< ClassName* >( arrayObj ); \
    	data_.resize( array.length() ); \
    	memcpy( data_[ 0 ] + start, &array[ 0 ], array->length * sizeof( Type ) ); \
    } \
    void set( const QVector< Type >& data ) { \
        data_ = std::vector< Type >( data.constData(), data.constData() + data.length ); \
    } \
    quint64 length() const { return data_.size(); } \
    void resize( quint64 sz ) { data_.resize( sz ); } \
    void clear() { data_.clear(); } \
    void push( Type v ) { data_.push_back( v ); } \
    void insert( quint64 start, QObject* arrayObj ) { \
    	if( !qobject_cast< ClassName* >( arrayObj ) ) { \
    	    error( "Not a LocoArray type" ); \
    	    return; \
    	} \
    	ClassName* array = qobject_cast< ClassName* >( arrayObj ); \
        data_.resize( data_.size() + array.size() ); \
        Type* base = &data_[ 0 ]; \
        memmove( base + start + array.size(), base + start, array.size() * sizeof( Type ) ); \
        memcpy(  base + start, &array_[ 0 ], array->size() * sizeof( type ) ); \
    } \
    void reserve( quint64 sz ) { data_.reserve( sz ); } \
    void remove( quint64 start, quint64 length ) { \
        data_.erase( data_.begin() + start, data_.begin() + start + length ); \
    } \
    QVariant clone() const { \
        ClassName* v = new ClassName ( *this ); \
        return GetContext()->AddObjToJSContext( v ); \
    } \
    QVariant extract( quint64 start, quint64 length ) { \
        ClassName* v = new ClassName( *this, start, length ); \
        remove( start, length ); \
        return GetContext()->AddObjToJSContext( v ); \
    } \
private: \
    std::vector< Type > data_; \
};

}

