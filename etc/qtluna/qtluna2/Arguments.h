#pragma once

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <QString>
#include <QVariantMap>
#include <QVariantList>
#include <QString>
#include <QList>
#include <QGenericArgument>
#include <QGenericReturnArgument>
#include <QVector>

#include "LuaQtTypes.h"

namespace qlua {

	//------------------------------------------------------------------------------
struct IArgConstructor {
	virtual QGenericArgument Create( lua_State*, int ) const = 0;
	virtual ~IArgConstructor() {}
	virtual IArgConstructor* Clone() const = 0;
	QGenericArgument ga_;
};

struct IntArgConstructor : IArgConstructor {
	QGenericArgument Create( lua_State* L, int idx ) const {
	    i_ = luaL_checkint( L, idx );
		return Q_ARG( int, i_ );
 	}
	IntArgConstructor* Clone() const {
	    return new IntArgConstructor( *this );
	}
	mutable int i_;
};
struct FloatArgConstructor : IArgConstructor {
	QGenericArgument Create( lua_State* L, int idx ) const {
	    f_ = float( luaL_checknumber( L, idx ) );
		return Q_ARG( double, f_ );
 	}
	FloatArgConstructor* Clone() const {
	    return new FloatArgConstructor( *this );
	}
	mutable float f_;
};
struct DoubleArgConstructor : IArgConstructor {
	QGenericArgument Create( lua_State* L, int idx ) const {
	    d_ = luaL_checknumber( L, idx );
		return Q_ARG( double, d_ );
 	}
	DoubleArgConstructor* Clone() const {
	    return new DoubleArgConstructor( *this );
	}
	mutable double d_;
};
struct StringArgConstructor : IArgConstructor {
	QGenericArgument Create( lua_State* L, int idx ) const {
		s_ = luaL_checkstring( L, idx ); 
	    return Q_ARG( QString, s_ );
 	}
	StringArgConstructor* Clone() const {
	    return new StringArgConstructor( *this );
	}
	mutable QString s_;
};
struct VariantMapArgConstructor : IArgConstructor {
    QGenericArgument Create( lua_State* L, int idx ) const {
		vm_ = ParseLuaTable( L, idx );
	    return Q_ARG( QVariantMap, vm_ );
 	}
	VariantMapArgConstructor* Clone() const {
	    return new VariantMapArgConstructor( *this );
	}
	mutable QVariantMap vm_;
};
struct VariantListArgConstructor : IArgConstructor {
	QGenericArgument Create( lua_State* L, int idx ) const {
		vl_ = ParseLuaTableAsVariantList( L, idx );
	    return Q_ARG( QVariantList, vl_ );
 	}
	VariantListArgConstructor* Clone() const {
	    return new VariantListArgConstructor( *this );
	}
	mutable QVariantList vl_;
};
struct ObjectStarArgConstructor : IArgConstructor {
	QGenericArgument Create( lua_State* L, int idx ) const {
		if( lua_istable( L, idx ) ) {
			lua_pushstring( L, "qobject__" );
			lua_gettable( L, idx );
			obj_ = reinterpret_cast< QObject* >( lua_touserdata( L, -1 ) );
			lua_pop( L, 1 );
		} else if( lua_islightuserdata( L, idx ) ) {
		    obj_ = reinterpret_cast< QObject* >( lua_touserdata( L, -1 ) );
		}
	    return Q_ARG( QObject*, obj_ );
 	}
	ObjectStarArgConstructor* Clone() const {
	    return new ObjectStarArgConstructor( *this );
	}
	mutable QObject* obj_;
};
struct WidgetStarArgConstructor : IArgConstructor {
	QGenericArgument Create( lua_State* L, int idx ) const {
		if( lua_istable( L, idx ) ) {
			lua_pushstring( L, "qobject__" );
			lua_gettable( L, idx );
			w_ = reinterpret_cast< QWidget* >( lua_touserdata( L, -1 ) );
			lua_pop( L, 1 );
		} else if( lua_islightuserdata( L, idx ) ) {
		    w_ = reinterpret_cast< QWidget* >( lua_touserdata( L, -1 ) );
		}
	    return Q_ARG( QWidget*, w_ );
 	}
	WidgetStarArgConstructor* Clone() const {
	    return new WidgetStarArgConstructor( *this );
	}
	mutable QWidget* w_;
};
struct VoidStarArgConstructor : IArgConstructor {
	QGenericArgument Create( lua_State* L, int idx ) const {
		v_ = const_cast< void* >( lua_topointer( L, idx ) );
	    return Q_ARG( void*, v_ );
 	}
	VoidStarArgConstructor* Clone() const {
	    return new VoidStarArgConstructor( *this );
	}
	mutable void* v_;
};
template< typename T >
struct ListArgConstructor : IArgConstructor {
	QGenericArgument Create( lua_State* L, int idx ) const {
		l_ = ParseLuaTableAsNumberList< T >( L, idx );
	    return Q_ARG( QList< T >, l_ );
 	}
	ListArgConstructor* Clone() const {
	    return new ListArgConstructor< T >( *this );
	}
	mutable QList< T > l_;
};
template< typename T >
struct VectorArgConstructor : IArgConstructor {
	QGenericArgument Create( lua_State* L, int idx ) const {
		v_ = ParseLuaTableAsNumberVector< T >( L, idx );
	    return Q_ARG( QVector< T >, v_ );
 	}
	VectorArgConstructor* Clone() const {
	    return new VectorArgConstructor< T >( *this );
	}
	mutable QVector< T > v_;
};

//------------------------------------------------------------------------------
class ReturnConstructor {
public:
    virtual void Push( lua_State* ) const = 0;
	virtual ~ReturnConstructor() {}
	virtual ReturnConstructor* Clone() const = 0;
	virtual QMetaType::Type Type() const = 0;
	QGenericReturnArgument Argument() const { return ga_; }
	template < typename T > void SetArg( T& arg ) {
		ga_ = QReturnArgument< T >( QMetaType::typeName( Type() ), arg );
	}
protected:
	QGenericReturnArgument ga_; 
};
class IntReturnConstructor : public ReturnConstructor {
public:
	IntReturnConstructor() {
	    SetArg( i_ );
	}
	IntReturnConstructor( const IntReturnConstructor& other ) : i_( other.i_ ) {
	    SetArg( i_ );
	}
	void Push( lua_State* L ) const {
		lua_pushinteger( L, i_ );
	}
	IntReturnConstructor* Clone() const {
	    return new IntReturnConstructor( *this );
	}
	QMetaType::Type Type() const { return QMetaType::Int; }
private:
	int i_;	
};
class DoubleReturnConstructor : public ReturnConstructor {
public:
	DoubleReturnConstructor() {
	   SetArg( d_ ); 
	}
	DoubleReturnConstructor( const DoubleReturnConstructor& other ) : d_( other.d_ ) {
	   SetArg( d_ );
	}
	void Push( lua_State* L ) const {
	    lua_pushnumber( L, d_ );
	}
	DoubleReturnConstructor* Clone() const {
	    return new DoubleReturnConstructor( *this );
	}
	QMetaType::Type Type() const { return QMetaType::Double; }
private:
	double d_;
};
class FloatReturnConstructor : public ReturnConstructor {
public:
	FloatReturnConstructor() {
	   SetArg( f_ ); 
	}
	FloatReturnConstructor( const FloatReturnConstructor& other ) : f_( other.f_ ) {
	   SetArg( f_ );
	}
	void Push( lua_State* L ) const {
	    lua_pushnumber( L, f_ );
	}
	FloatReturnConstructor* Clone() const {
	    return new FloatReturnConstructor( *this );
	}
	QMetaType::Type Type() const { return QMetaType::Float; }
private:
	float f_;
};
class StringReturnConstructor : public ReturnConstructor {
public:
	StringReturnConstructor() {
		SetArg( s_ );
	}
	StringReturnConstructor( const StringReturnConstructor& other ) : s_( other.s_ ) {
		SetArg( s_ );
	}
	void Push( lua_State* L ) const {
		lua_pushstring( L, s_.toAscii().constData() );
	}
	StringReturnConstructor* Clone() const {
	    return new StringReturnConstructor( *this );
	}
	QMetaType::Type Type() const { return QMetaType::QString; }
private:
	QString s_;
};
class VoidReturnConstructor : public ReturnConstructor {
public:
	void Push( lua_State*  ) const {}
	VoidReturnConstructor* Clone() const {
	    return new VoidReturnConstructor( *this );
	}
	QMetaType::Type Type() const { return QMetaType::Void; }
};
class VariantMapReturnConstructor : public ReturnConstructor {
public:
	VariantMapReturnConstructor() {
	    SetArg( vm_ );   
	}
	VariantMapReturnConstructor( const VariantMapReturnConstructor& other ) : vm_( other.vm_ ) {
	    SetArg( vm_ );
	}
	void Push( lua_State* L ) const {
		VariantMapToLuaTable( vm_, L );
	}
	VariantMapReturnConstructor* Clone() const {
	    return new VariantMapReturnConstructor( *this );
	}
	QMetaType::Type Type() const { return QMetaType::QVariantMap; }
private:
	QVariantMap vm_;
};
class VariantListReturnConstructor : public ReturnConstructor {
public:
	VariantListReturnConstructor() {
	    SetArg( vl_ );   
	}
	VariantListReturnConstructor( const VariantListReturnConstructor& other ) : vl_( other.vl_ ) {
	    SetArg( vl_ );
	}
	void Push( lua_State* L ) const {
		VariantListToLuaTable( vl_, L );
	}
	VariantListReturnConstructor* Clone() const {
	    return new VariantListReturnConstructor( *this );
	}
	QMetaType::Type Type() const { return QMetaType::QVariantList; }
private:
	QVariantList vl_;
};
class ObjectStarReturnConstructor : public ReturnConstructor {
public:
	ObjectStarReturnConstructor() {
	    SetArg( obj_ );   
	}
	ObjectStarReturnConstructor( const ObjectStarReturnConstructor& other ) : obj_( other.obj_ ) {
	    SetArg( obj_ );
	}
	void Push( lua_State* L ) const {
		lua_pushlightuserdata( L, obj_ );
	}
	ObjectStarReturnConstructor* Clone() const {
	    return new ObjectStarReturnConstructor( *this );
	}
	QMetaType::Type Type() const { return QMetaType::QObjectStar; }
private:
	QObject* obj_;
};
class WidgetStarReturnConstructor : public ReturnConstructor {
public:
	WidgetStarReturnConstructor() {
	    SetArg( w_ );   
	}
	WidgetStarReturnConstructor( const WidgetStarReturnConstructor& other ) : w_( other.w_ ) {
	    SetArg( w_ );
	}
	void Push( lua_State* L ) const {
		lua_pushlightuserdata( L, w_ );
	}
	WidgetStarReturnConstructor* Clone() const {
	    return new WidgetStarReturnConstructor( *this );
	}
	QMetaType::Type Type() const { return QMetaType::QWidgetStar; }
private:
	QWidget* w_;
};
class VoidStarReturnConstructor : public ReturnConstructor {
public:
	VoidStarReturnConstructor() {
	    SetArg( v_ );   
	}
	VoidStarReturnConstructor( const VoidStarReturnConstructor& other ) : v_( other.v_ ) {
	    SetArg( v_ );
	}
	void Push( lua_State* L ) const {
		lua_pushlightuserdata( L, v_ );
	}
	VoidStarReturnConstructor* Clone() const {
	    return new VoidStarReturnConstructor( *this );
	}
	QMetaType::Type Type() const { return QMetaType::VoidStar; }
private:
	void* v_;
};

template < typename T >
class ListReturnConstructor : public ReturnConstructor {
public:
	ListReturnConstructor() {
	    SetArg( l_ );   
	}
	ListReturnConstructor( const ListReturnConstructor& other ) : l_( other.l_ ) {
	    SetArg( l_ );
	}
	void Push( lua_State* L ) const {
		NumberListToLuaTable< T >( l_, L );
	}
	ListReturnConstructor* Clone() const {
	    return new ListReturnConstructor( *this );
	}
	QMetaType::Type Type() const { return QMetaType::Type( QMetaType::type( TypeName< QList< T > >() ) ); }
private:
	QList< T > l_;
};

template < typename T >
class VectorReturnConstructor : public ReturnConstructor {
public:
	VectorReturnConstructor() {
	    SetArg( v_ );   
	}
	VectorReturnConstructor( const VectorReturnConstructor& other ) : v_( other.v_ ) {
	    SetArg( v_ );
	}
	void Push( lua_State* L ) const {
		NumberVectorToLuaTable< T >( v_, L );
	}
	VectorReturnConstructor* Clone() const {
	    return new VectorReturnConstructor( *this );
	}
	QMetaType::Type Type() const { return QMetaType::Type( QMetaType::type( TypeName< QVector< T > >() ) ); }
private:
	QVector< T > v_;
};


//------------------------------------------------------------------------------
class ParameterWrapper {
public:
	ParameterWrapper() : ac_( 0 ) {}
	ParameterWrapper( const ParameterWrapper& other ) : ac_( 0 ) {
		if( other.ac_ ) ac_ = other.ac_->Clone();
	}
	ParameterWrapper( const QString& type ) : ac_( 0 ) {
		if( type == QMetaType::typeName( QMetaType::Int ) ) {
			ac_ = new IntArgConstructor;
		} else if( type == QMetaType::typeName( QMetaType::Double ) ) {
			ac_ = new DoubleArgConstructor;
		} else if( type == QMetaType::typeName( QMetaType::Float ) ) {
			ac_ = new FloatArgConstructor;
		} else if( type == QMetaType::typeName( QMetaType::QString ) ) {
			ac_ = new StringArgConstructor;
		} else if( type == QMetaType::typeName( QMetaType::QVariantMap ) ) {
			ac_ = new VariantMapArgConstructor;
		} else if( type == QMetaType::typeName( QMetaType::QVariantList ) ) {
			ac_ = new VariantListArgConstructor;
		} else if( type == QMetaType::typeName( QMetaType::QObjectStar ) ) {
			ac_ = new ObjectStarArgConstructor;
		} else if( type == QMetaType::typeName( QMetaType::QWidgetStar ) ) {
			ac_ = new WidgetStarArgConstructor;
		} else if( type == QMetaType::typeName( QMetaType::VoidStar ) ) {
			ac_ = new VoidStarArgConstructor;
		} else if( type == QLUA_LIST_FLOAT64 ) {
			ac_ = new ListArgConstructor< double >;
		} else if( type == QLUA_LIST_FLOAT32 ) {
			ac_ = new ListArgConstructor< float >;
		} else if( type == QLUA_LIST_INT ) {
			ac_ = new ListArgConstructor< int >;
		} else if( type == QLUA_LIST_SHORT ) {
			ac_ = new ListArgConstructor< short >;
		} else if( type == QLUA_VECTOR_FLOAT64 ) {
			ac_ = new VectorArgConstructor< double >;
		} else if( type == QLUA_VECTOR_FLOAT32 ) {
			ac_ = new VectorArgConstructor< float >;
		} else if( type == QLUA_VECTOR_INT ) {
			ac_ = new VectorArgConstructor< int >;
		} else if( type == QLUA_VECTOR_SHORT ) {
			ac_ = new VectorArgConstructor< short >;
		} else throw std::logic_error( ( "Type " + type + " unknown" ).toStdString() );
	}
    QGenericArgument Arg( lua_State* L, int idx ) const {
		return ac_ ? ac_->Create( L, idx ) : QGenericArgument();
    }
    ~ParameterWrapper() { delete ac_; }
private:
    IArgConstructor* ac_;	
};

class ReturnWrapper {
public:
	ReturnWrapper() : rc_( 0 ) {}
	ReturnWrapper( const ReturnWrapper& other ) : rc_( 0 ), type_( other.type_ ) {
		if( other.rc_ ) rc_ = other.rc_->Clone();
	}
	ReturnWrapper( const QString& type ) : rc_( 0 ), type_( type ) {
		if( type_ == QMetaType::typeName( QMetaType::Int ) ) {
			rc_ = new IntReturnConstructor;
		} else if( type == QMetaType::typeName( QMetaType::Double ) ) {
			rc_ = new DoubleReturnConstructor;
		} else if( type == QMetaType::typeName( QMetaType::Float ) ) {
			rc_ = new FloatReturnConstructor;
		} else if( type_ == QMetaType::typeName( QMetaType::QString ) ) {
			rc_ = new StringReturnConstructor;
		} else if( type_ == QMetaType::typeName( QMetaType::QVariantMap ) ) {
			rc_ = new VariantMapReturnConstructor;
		} else if( type_ == QMetaType::typeName( QMetaType::QVariantList ) ) {
			rc_ = new VariantListReturnConstructor;
		} else if( type == QMetaType::typeName( QMetaType::QObjectStar ) ) {
			rc_ = new ObjectStarReturnConstructor;
		} else if( type == QMetaType::typeName( QMetaType::QWidgetStar ) ) {
			rc_ = new WidgetStarReturnConstructor;
		} else if( type == QMetaType::typeName( QMetaType::VoidStar ) ) {
			rc_ = new VoidStarReturnConstructor;
		} else if( type == QLUA_LIST_FLOAT64 ) {
			rc_ = new ListReturnConstructor< double >;
		} else if( type == QLUA_LIST_FLOAT32 ) {
			rc_ = new ListReturnConstructor< float >;
		} else if( type == QLUA_LIST_INT ) {
			rc_ = new ListReturnConstructor< int >;
		} else if( type == QLUA_LIST_SHORT ) {
			rc_ = new ListReturnConstructor< short >;
		} else if( type == QLUA_VECTOR_FLOAT64 ) {
			rc_ = new VectorReturnConstructor< double >;
		} else if( type == QLUA_VECTOR_FLOAT32 ) {
			rc_ = new VectorReturnConstructor< float >;
		} else if( type == QLUA_VECTOR_INT ) {
			rc_ = new VectorReturnConstructor< int >;
		} else if( type == QLUA_VECTOR_SHORT ) {
			rc_ = new VectorReturnConstructor< short >;
		} else if( type_.isEmpty() ) rc_ = new VoidReturnConstructor;
	    else throw std::logic_error( ( "Type " + type + " unknown" ).toStdString() );
	}
    void Push( lua_State* L ) const {
    	rc_->Push( L );
    }
	QGenericReturnArgument Arg() const { return rc_->Argument(); }
	const QString& Type() const { 
		return type_;
	}
	QMetaType::Type MetaType() const { return rc_->Type(); }
    ~ReturnWrapper() { delete rc_; }
private:
    ReturnConstructor* rc_;
	QString type_;
};

typedef QList< ParameterWrapper > ParamWrappers;
typedef QList< QByteArray > ParamTypes;
inline ParamWrappers GenerateParameterWrappers( const ParamTypes& pt ) {
	ParamWrappers pw;
	for( ParamTypes::const_iterator i = pt.begin(); i != pt.end(); ++i ) {
	    pw.push_back( ParameterWrapper( *i ) );
	}
	return pw;
}

inline ReturnWrapper GenerateReturnWrapper( const QString& typeName ) {
    return ReturnWrapper( typeName );
}

}