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
//template < typename T >
//struct ListArgConstructor : IArgConstructor {
//	QGenericArgument Create( lua_State* L, int idx ) const {
//		l = ParseLuaTableAsList< T >( L, idx );
//	    return Q_ARG( QString, l_ );
// 	}
//	VariantListArgConstructor* Clone() const {
//	    return new VariantListArgConstructor( *this );
//	}
//	mutable QList< T > l_;
//};

//------------------------------------------------------------------------------
struct IReturnConstructor {
    virtual void Push( lua_State* ) const = 0;
	virtual ~IReturnConstructor() {}
	virtual IReturnConstructor* Clone() const = 0;
	QGenericReturnArgument ga_; 
};
struct IntReturnConstructor : IReturnConstructor {
	IntReturnConstructor() {
	    ga_ = Q_RETURN_ARG( int, i_ );
	}
	void Push( lua_State* L ) const {
		lua_pushinteger( L, i_ );
	}
	IntReturnConstructor* Clone() const {
	    return new IntReturnConstructor( *this );
	}
	int i_;	
};
struct DoubleReturnConstructor : IReturnConstructor {
	DoubleReturnConstructor() {
	   ga_ = Q_RETURN_ARG( double, d_ ); 
	}
	void Push( lua_State* L ) const {
	    lua_pushnumber( L, d_ );
	}
	DoubleReturnConstructor* Clone() const {
	    return new DoubleReturnConstructor( *this );
	}
	double d_;
};
struct FloatReturnConstructor : IReturnConstructor {
	FloatReturnConstructor() {
	   ga_ = Q_RETURN_ARG( float, f_ ); 
	}
	void Push( lua_State* L ) const {
	    lua_pushnumber( L, f_ );
	}
	FloatReturnConstructor* Clone() const {
	    return new FloatReturnConstructor( *this );
	}
	float f_;
};
struct StringReturnConstructor : IReturnConstructor {
	StringReturnConstructor() {
	    ga_ = Q_RETURN_ARG( QString, s_ );   
	}
	void Push( lua_State* L ) const {
		lua_pushstring( L, s_.toAscii().constData() );
	}
	StringReturnConstructor* Clone() const {
	    return new StringReturnConstructor( *this );
	}
	QString s_;
};
struct VoidReturnConstructor : IReturnConstructor {
	void Push( lua_State*  ) const {}
	VoidReturnConstructor* Clone() const {
	    return new VoidReturnConstructor( *this );
	}
};
struct VariantMapReturnConstructor : IReturnConstructor {
	VariantMapReturnConstructor() {
	    ga_ = Q_RETURN_ARG( QVariantMap, vm_ );   
	}
	void Push( lua_State* L ) const {
		VariantMapToLuaTable( vm_, L );
	}
	VariantMapReturnConstructor* Clone() const {
	    return new VariantMapReturnConstructor( *this );
	}
	QVariantMap vm_;
};
struct VariantListReturnConstructor : IReturnConstructor {
	VariantListReturnConstructor() {
	    ga_ = Q_RETURN_ARG( QVariantList, vl_ );   
	}
	void Push( lua_State* L ) const {
		VariantListToLuaTable( vl_, L );
	}
	VariantListReturnConstructor* Clone() const {
	    return new VariantListReturnConstructor( *this );
	}
	QVariantList vl_;
};
//template < typename T >
//struct ListReturnConstructor : IReturnConstructor {
//	ListReturnConstructor() {
//	    ga_ = Q_RETURN_ARG( QList< T >, l_ );   
//	}
//	void Push( lua_State* L ) const {
//		ListToLuaTable< T >( L, l_ );
//	}
//	ListReturnConstructor* Clone() const {
//	    return new ListReturnConstructor( *this );
//	}
//	QList< T > vl_;
//};

//------------------------------------------------------------------------------
class ParameterWrapper {
public:
	ParameterWrapper() : ac_( 0 ) {}
	ParameterWrapper( const ParameterWrapper& other ) : ac_( 0 ) {
		if( other.ac_ ) ac_ = other.ac_->Clone();
	}
	ParameterWrapper( const QString& type ) : ac_( 0 ) {
		if( type == "int" ) {
			ac_ = new IntArgConstructor;
		} else if( type == "double" ) {
			ac_ = new DoubleArgConstructor;
		} else if( type == "QString" ) {
			ac_ = new StringArgConstructor;
		}
	}
    QGenericArgument Arg( lua_State* L, int idx ) const {
    	return ac_->Create( L, idx );
    }
    ~ParameterWrapper() { delete ac_; }
private:
    IArgConstructor* ac_;	
};

class ReturnWrapper {
public:
	ReturnWrapper() : rc_( 0 ) {}
	ReturnWrapper( const ReturnWrapper& other ) : rc_( 0 ) {
		if( other.rc_ ) rc_ = other.rc_->Clone();
	}
	ReturnWrapper( const QString& type ) : rc_( 0 ), type_( type ) {
		if( type_ == "int" ) {
			rc_ = new IntReturnConstructor;
		} else if( type_ == "double" ) {
			rc_ = new DoubleReturnConstructor;
		} else if( type_ == "QString" ) {
			rc_ = new StringReturnConstructor;
		} else if( type_.isEmpty() ) rc_ = new VoidReturnConstructor;
	}
    void Push( lua_State* L ) const {
    	rc_->Push( L );
    }
	QGenericReturnArgument Arg() const { return rc_->ga_; }
	const QString& Type() const { return type_; }
    ~ReturnWrapper() { delete rc_; }
private:
    IReturnConstructor* rc_;
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