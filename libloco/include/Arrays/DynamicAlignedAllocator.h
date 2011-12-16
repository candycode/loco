#pragma once
//#SRCHEADER

#include <new> //bad_alloc
#include <cstdlib> //malloc, free
#include <cassert> //assert
#include <limits> //max

///\warning INTENDED FOR POD TYPES ONLY
/// The only changes required to make it work with any type
/// are to explicitly invoke destructors in the deallocate() abd destroy() methods
/// and use placement new in construct()

template <class T > class DynamicAlignedAllocator {
public:
	typedef size_t    size_type;
	typedef ptrdiff_t difference_type;
	typedef T*        pointer;
	typedef const T*  const_pointer;
	typedef T&        reference;
	typedef const T&  const_reference;
	typedef T         value_type;
	template <class U> struct rebind { typedef DynamicAlignedAllocator<U> other; };
public:
	DynamicAlignedAllocator( size_t alignment = 0 ) throw() : alignment_( alignment ) {
		assert( sizeof( void* ) == sizeof( size_t ) );
		assert( sizeof( quint64 ) >= sizeof( size_t ) );
	}
	DynamicAlignedAllocator(const DynamicAlignedAllocator& other ) throw() : alignment_( other.alignment_ ) {}
	template <class U> DynamicAlignedAllocator( const DynamicAlignedAllocator<U>& other ) throw()  :  alignment_( other.alignment() ) {}
	~DynamicAlignedAllocator() throw() {}
	pointer address( reference x ) const { return &x; }
	const_pointer address( const_reference x ) const { return &x; };
	pointer allocate( size_type sz, const void* = 0 ) {
        if( alignment_ == 0 ) {
        	pointer p = reinterpret_cast< pointer >( malloc( sz * sizeof( value_type ) ) );
        	if( p == 0 ) throw std::bad_alloc();
        	return p;
        }
		char* buffer =
			  reinterpret_cast< char* >( malloc( sz * sizeof( value_type ) +
												 sizeof( size_t ) + alignment_ ) );
		if( buffer == 0 ) {
		  throw std::bad_alloc();
		  return 0; // in case exceptions are not enabled
		}
		const size_t offset = size_t( buffer + sizeof( size_t ) );
		char* ptr = reinterpret_cast< char* >( offset + ( alignment_ - offset  % alignment_ ) );
		size_t* start = reinterpret_cast< size_t* >( ptr - sizeof( size_t ) );
		*start = size_t( buffer );
		return reinterpret_cast< pointer >( ptr );
	}
	void deallocate(pointer p, size_type /*num_elements*/ ) {
		if( alignment_ == 0 ) {
			free( p );
			return;
		}
		size_t* start = reinterpret_cast< size_t* >( p - sizeof( size_t ) );
			    free( reinterpret_cast< void* >( *start ) );
		//iterate over num_elements and destroy one by one through destroy() method
	}
	size_type max_size() const throw() {
	    return std::numeric_limits< size_t >::max();
	}
	void construct( pointer p, const T& val ) {
	    //new ( p ) T( val );
		*p = val;
	}
	void destroy(pointer /*p*/) {
        //p->~T();
	}
public:
	size_t alignment() const { return alignment_; }
	void set_alignment( size_t a ) { alignment_ = a; }
private:
	size_t alignment_;
};

template< typename T >
bool operator==(const DynamicAlignedAllocator< T >& a1,
		        const DynamicAlignedAllocator< T >& a2 ) { return  a1.alignment() == a2.alignment(); }
template< typename T >
bool operator!=(const DynamicAlignedAllocator< T >& a1,
		        const DynamicAlignedAllocator< T >& a2 ) { return  a1.alignment() != a2.alignment(); }

