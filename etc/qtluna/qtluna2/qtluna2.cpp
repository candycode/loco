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
#include <QSet>
#include <QMetaType>

#include "MyObject.h"

#include "DynamicLuaQObject.h"

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

//------------------------------------------------------------------------------
struct IReturnConstructor {
    virtual void Push( lua_State* ) const = 0;
	virtual ~IReturnConstructor() {}
	virtual IReturnConstructor* Clone() const = 0;
	QGenericArgument ga_; 
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
		lua_pushlightuserdata( L_, this );
		lua_pushcclosure( L_, &LuaContext::QtConnect , 1);
		lua_setglobal( L_, "qtconnect" );
	}
	void Eval( const char* code ) {
		ReportErrors( luaL_dostring( L_, code ) );
	}
	void AddQObject( QObject* obj, 
		             const char* tableName,
					 const QSet< QString >& methodNames = QSet< QString >(),
					 const QSet< QMetaMethod::MethodType >& methodTypes = QSet< QMetaMethod::MethodType >()  ) {
        lua_newtable( L_ );
		const QMetaObject* mo = obj->metaObject();
		for( int i = 0; i != mo->methodCount(); ++i ) {
			QMetaMethod mm = mo->method( i );
			QString name = mm.signature();
			name.truncate( name.indexOf("(") );
			if( !methodNames.isEmpty() && !methodNames.contains( name ) ) continue;
			if( !methodTypes.isEmpty() && !methodTypes.contains( mm.methodType() ) ) continue;
			typedef QList< QByteArray > Params;
			Params params = mm.parameterTypes();
			const QString returnType = mm.typeName();
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
		lua_pushstring( L_, "qobject__" );
		lua_pushlightuserdata( L_, obj );
        lua_rawset( L_, -3 );

		//add method return pointer to object
		lua_setglobal( L_, tableName );
	}
	~LuaContext() {
		lua_close( L_ );
	}
private:
	static int QtConnect( lua_State* L ) {
		LuaContext& lc = *reinterpret_cast< LuaContext* >( lua_touserdata( L, lua_upvalueindex( 1 ) ) );
		if( lua_gettop( L ) != 3 ) {
		    lua_pushstring( L, "Three parameters required" );
		    lua_error( L );
			return 0;
		}
		if( !lua_istable( L, 1 ) ) {
		    lua_pushstring( L, "First parameter to function 'qtconnect' is not a table" );
		    lua_error( L );
			return 0;
		}
		lua_pushstring( L, "qobject__" );
		lua_gettable( L, 1 );
		if( lua_isnil( L, -1 ) ) {
			lua_pushstring( L, "Wrong table format: reference to QObject not found" );
		    lua_error( L );
			return 0;
		}
		QObject* obj = reinterpret_cast< QObject* >( lua_touserdata( L, -1 ) );
		const QString signal = lua_tostring( L, 2 );
		//extract signal arguments info
		const QMetaObject* mo = obj->metaObject();
		QList< QByteArray > params;
		QString signalSignature;
		for( int i = 0; i != mo->methodCount(); ++i ) {
			QMetaMethod mm = mo->method( i );
			QString name = mm.signature();
			name.truncate( name.indexOf("(") );
			if( name == signal ) {
				signalSignature = mm.signature();
				params = mm.parameterTypes();                  
			    break;
			}
		}
		QList< QMetaType::Type > types;
		for( QList< QByteArray >::const_iterator i = params.begin();
			 i != params.end(); ++i ) {
				 types.push_back( QMetaType::Type( QMetaType::type( i->constData() ) ) );
		}
		//push lua callback onto top of stack
		lua_pushvalue( L, 3 );
		const int luaRef = luaL_ref( L, LUA_REGISTRYINDEX );
		const QString slot = signal + "_" + QString("%1").arg( luaRef );
		lc.dispatcher_.RegisterSlot( slot, types, luaRef );
		lc.dispatcher_.connectDynamicSlot( obj, signalSignature.toAscii().data(), slot.toAscii().data() ); 
        return 0;
	}
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
			case 0: return Invoke0( mi, L );
			        break;
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
	static int Invoke0( const Method* mi, lua_State* L ) {
		bool ok = false;
		if( mi->returnWrapper_.Type().isEmpty() ) {
			ok = mi->metaMethod_.invoke( mi->obj_, Qt::DirectConnection );
		    if( ok ) return 0;
		} else {
			ok = mi->metaMethod_.invoke( mi->obj_, Qt::DirectConnection,
	    		                         mi->returnWrapper_.Arg() ); //passes the location (void*) where return data will be stored
			if( ok ) {
				mi->returnWrapper_.Push( L );
			    return 1;
			}
		}
		lua_pushstring( L, "Slot invocation error" );
		lua_error( L );
		return 0;
    }
    static int Invoke1( const Method* mi, lua_State* L ) {
		bool ok = false;
		if( mi->returnWrapper_.Type().isEmpty() ) {
			ok = mi->metaMethod_.invoke( mi->obj_, Qt::DirectConnection,
	          			                 mi->paramWrappers_[ 0 ].Arg( L, 1 ) );
		    if( ok ) return 0;
		} else {
			ok = mi->metaMethod_.invoke( mi->obj_, Qt::DirectConnection,
	    		                         mi->returnWrapper_.Arg(), //passes the location (void*) where return data will be stored
	          			                 mi->paramWrappers_[ 0 ].Arg( L, 1 ) );
			if( ok ) {
				mi->returnWrapper_.Push( L );
			    return 1;
			}
		}
		lua_pushstring( L, "Slot invocation error" );
		lua_error( L );
		return 0;
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
	DynamicLuaQObject dispatcher_; //signal->dispatcher_->Lua callback
};


//------------------------------------------------------------------------------
int main() {
	try {
	    LuaContext ctx;
        MyObject myobj;
        ctx.AddQObject( &myobj, "myobj" );
	    ctx.Eval( "qtconnect( myobj, 'aSignal', function() print( 'called!' ); end )" );
        ctx.Eval( "myobj.emitSignal()" ); 
	} catch( const std::exception& e ) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}