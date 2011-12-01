#pragma once
//#SRCHEADER

namespace loco {

class ArrayBuffer : public Object {
	Q_OBJECT
public:
    char* Data() { return &(*buffer_.begin()); }
};

class Int32Buffer : public Ob
	Q_OBJECT
public:
public slots:
    int at( quint64 idx ) {
        return data_[ idx ];
    }
    void set( int idx, int v ) { data_[ idx ] = v; }
    quint64 length() const { return data_.size(); }
    void resize( quint64 sz ) { data_.resize( sz ); }
    void clear() { data_.clear(); }
    void push( int v ) { data_.push_back( v ); }
    Int32Buffer slice( quint64 start, quint64 size ) {}

};

#define LOCO_CREATE_ARRAY_BUFFER( name, type ) \
class name : public Object { \
	Q_OBJECT \
public:
public slots: \
    type at( quint64 idx ) { \
        return data_[ idx ];
    } \
    void set( quint64 idx, type v ) { data_[ idx ] = v; } \
    quint64 length() const { return data_.size(); } \
    void resize( quint64 sz ) { data_.resize( sz ); } \
    void clear() { data_.clear(); } \
    void push( type v ) { data_.push_back( v ); } \
    name slice( quint64 start, quint64 size ) {} \
    void insert( quint64 start, const name & array ) {
        data_.resize( data_.size() + array.size() ); \
        type* base = &data_[ 0 ]; \
        memmove( base + start, base + start + array.size(), array.size() * sizeof( type ) ); \
        memcpy( &array[ 0 ], base + start, array.size() * sizeof( type ) ); \
    } \
    void reserve( quint64 sz ) { data_.reserve( sz ); } \
    void remove( base.start)
private: \
    std::vector< type > data_; \
};




}

