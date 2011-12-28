extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <stdexcept>
#include <string>

#include <QString>
#include <QMap>
#include <QList>
#include <QMetaObject>
#include <QMetaMethod>

#include "MyObject.h"


//------------------------------------------------------------------------------
struct IArgConstructor {
	virtual QGenericArgument Create( lua_State*, int ) const = 0;
	virtual ~IArgConstructor() {}
	QGenericArgument ga_;
};

struct IntArgConstructor : IArgConstructor {
	QGenericArgument Create( lua_State* L, int idx ) const {
	    i_ = luaL_checkint( L, idx );
		return Q_ARG( int, i_ );
 	}
	mutable int i_;
};
struct DoubleArgConstructor : IArgConstructor {
	QGenericArgument Create( lua_State* L, int idx ) const {
	    d_ = luaL_checknumber( L, idx );
		return Q_ARG( double, d_ );
 	}
	mutable double d_;
};
struct StringArgConstructor : IArgConstructor {
	QGenericArgument Create( lua_State* L, int idx ) const {
		s_ = luaL_checkstring( L, idx ); 
	    return Q_ARG( QString, s_ );
 	}
	mutable QString s_;
};

//------------------------------------------------------------------------------
struct IReturnConstructor {
    virtual void Push( lua_State* ) const = 0;
	virtual ~IReturnConstructor() {}
	QGenericArgument ga_; 
};
struct IntReturnConstructor : IReturnConstructor {
	IntReturnConstructor() {
	    ga_ = Q_RETURN_ARG( int, i_ );
	}
	void Push( lua_State* L ) const {
		lua_pushinteger( L, i_ );
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
	double d_;
};
struct StringReturnConstructor : IReturnConstructor {
	StringReturnConstructor() {
	    ga_ = Q_RETURN_ARG( QString, s_ );   
	}
	void Push( lua_State* L ) const {
		lua_pushstring( L, s_.toAscii().constData() );
	}
	QString s_;
};
struct VoidReturnConstructor : IReturnConstructor {
	void Push( lua_State*  ) const {}
};

//------------------------------------------------------------------------------
class ParameterWrapper {
public:
	ParameterWrapper( const QString& type ) {
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
	QGenericArgument Arg() const { return rc_->ga_; }
	const QString& Type() const { return type_; }
    ~ReturnWrapper() { delete rc_; }
private:
    IReturnConstructor* rc_;
	QString type_;
};

struct Method {
	typedef QList< ParameterWrapper > ParamWrappers;
    QObject* obj_;
	QMetaMethod metaMethod_;
	ParamWrappers paramWrappers_;
	ReturnWrapper returnWrapper_;
	Method( QObject* obj, const QMetaMethod& mm, const ParamWrappers& pw, const ReturnWrapper& rw ) :
	obj_( obj ), metaMethod_( mm ), paramWrappers_( pw ), returnWrapper_( rw ) {}
};
typedef QList< QByteArray > ParamTypes;
Method::ParamWrappers GenerateParameterWrappers( const ParamTypes& pt ) {
	Method::ParamWrappers pw;
	for( ParamTypes::const_iterator i = pt.begin(); i != pt.end(); ++i ) {
	    pw.push_back( ParameterWrapper( *i ) );
	}
	return pw;
}

ReturnWrapper GenerateReturnWrapper( const QString& typeName ) {
    return ReturnWrapper( typeName );
}


//------------------------------------------------------------------------------
class LuaContext {
public:
	typedef QList< Method > Methods;
	typedef QMap< QString, Methods > MethodMap;
	LuaContext() {
		L_ = luaL_newstate();
		luaL_openlibs(L_);
	}
	void Eval( const char* code ) {
		ReportErrors( luaL_dostring( L_, code ) );
	}
	void AddQObject( QObject* obj, const char* tableName ) {
        lua_newtable( L_ );
		const QMetaObject* mo = obj->metaObject();
		for( int i = 0; i != mo->methodCount(); ++i ) {
			QMetaMethod mm = mo->method( i );
			typedef QList< QByteArray > Params;
			Params params = mm.parameterTypes();
			const QString returnType = mm.typeName();
			QString name = mm.signature();
			name.truncate( name.indexOf("(") );
			methods_[ name ].push_back( Method( obj,
			                            mm, 
			                            GenerateParameterWrappers( params ),
			                            GenerateReturnWrapper( returnType ) ) );
			if( methods_[ name ].size() == 1 ) {
				lua_pushstring( L_, name.toAscii().constData() );
		        lua_pushlightuserdata( L_, &methods_[ name ] );
		        lua_pushcclosure( L_, LuaContext::InvokeMethod, 1 );
		        lua_rawset( L_, -3 );
		    }	                            
		}
		lua_setglobal( L_, tableName );
	}
	~LuaContext() {
		lua_close( L_ );
	}
private:
	static int InvokeMethod( lua_State *L ) {
		const Methods& m = *( reinterpret_cast< Methods* >( lua_touserdata( L, lua_upvalueindex( 1 ) ) ) );
        const int numArgs = lua_gettop( L );
		int idx = -1;
		const Method* mi = 0;
		for( Methods::const_iterator i = m.begin(); i != m.end(); ++i ) {
			if( i->paramWrappers_.size() == numArgs ) {
			    mi = &( *i );
				break;
			}
		}
		if( !mi ) throw std::runtime_error( "Method not found" );
		switch( numArgs ) {
			/*case 0: return Invoke0( mi, L );
			        break;*/
			case 1: return Invoke1( mi, L );
			        break;
			/*case 2: return Invoke2( mi, L );
			        break;
			case 3: return Invoke3( mi, L );
			        break;
			case 4: return Invoke4( mi, L );
			        break;
			case 5: return Invoke5( mi, L );
			        break;
			case 7: return Invoke6( mi, L );
			        break;
            case 8: return Invoke7( mi, L );
			        break;
            case 9: return Invoke8( mi, L );
			        break; */
			default: break;                                                         
		}
		return 0;
	}
	//move invokers into Invokers struct
    static int Invoke1( const Method* mi, lua_State* L ) {
		if( mi->returnWrapper_.Type().isEmpty() ) {
			mi->metaMethod_.invoke( mi->obj_, Qt::DirectConnection,
	          			            mi->paramWrappers_[ 0 ].Arg( L, 1 ) );
		    return 0;
		} else {
			mi->metaMethod_.invoke( mi->obj_, Qt::DirectConnection,
	    		                    mi->returnWrapper_.Arg(), //passes the location (void*) where return data will be stored
	          			            mi->paramWrappers_[ 0 ].Arg( L, 1 ) );
			mi->returnWrapper_.Push( L );
			return 1;
		}
    }
    void ReportErrors( int status ) {
    	if( status != 0 ) {
    		std::string err = lua_tostring( L_, -1 );
    		lua_pop( L_, 1 );
    		throw std::runtime_error( err );
    	}
    }	
private:
    lua_State* L_;	
	MethodMap methods_;
};


//------------------------------------------------------------------------------
int main() {
	LuaContext ctx;
    MyObject myobj;
    ctx.AddQObject( &myobj, "myobj" );
    ctx.Eval( "myobj.method(\"hello\")" ); 
	return 0;
}