#pragma once
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
#include <QStringList>

#include "DynamicLuaQObject.h"

//==============================================================================
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
//struct VariantMapArgConstructor : IArgConstructor {
//	QGenericArgument Create( lua_State* L, int idx ) const {
//		vm = ParseLuaTable( L, idx );
//	    return Q_ARG( QString, vm_ );
// 	}
//	VariantMapArgConstructor* Clone() const {
//	    return new VariantMapArgConstructor( *this );
//	}
//	mutable QVariantMap vm_;
//};
//struct VariantListArgConstructor : IArgConstructor {
//	QGenericArgument Create( lua_State* L, int idx ) const {
//		vl = ParseLuaTableAsVariantList( L, idx );
//	    return Q_ARG( QString, vl_ );
// 	}
//	VariantListArgConstructor* Clone() const {
//	    return new VariantListArgConstructor( *this );
//	}
//	mutable QVariantList vl_;
//};
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
//struct VariantMapReturnConstructor : IReturnConstructor {
//	VariantMapReturnConstructor() {
//	    ga_ = Q_RETURN_ARG( QVariantMap, vm_ );   
//	}
//	void Push( lua_State* L ) const {
//		VariantMapToLuaTable( L, vm_ );
//	}
//	VariantMapReturnConstructor* Clone() const {
//	    return new VariantMapReturnConstructor( *this );
//	}
//	QVariantMap vm_;
//};
//struct VariantListReturnConstructor : IReturnConstructor {
//	VariantListReturnConstructor() {
//	    ga_ = Q_RETURN_ARG( QVariantList, vl_ );   
//	}
//	void Push( lua_State* L ) const {
//		VariantListToLuaTable( L, vl_ );
//	}
//	VariantListReturnConstructor* Clone() const {
//	    return new VariantListReturnConstructor( *this );
//	}
//	QVariantList vl_;
//};
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
	QGenericArgument Arg() const { return rc_->ga_; }
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
//==============================================================================

//==============================================================================
//------------------------------------------------------------------------------
class LuaContext {
    struct Method {
        QObject* obj_;
	    QMetaMethod metaMethod_;
	    ParamWrappers paramWrappers_;
	    ReturnWrapper returnWrapper_;
	    Method( QObject* obj, const QMetaMethod& mm, const ParamWrappers& pw, const ReturnWrapper& rw ) :
	    obj_( obj ), metaMethod_( mm ), paramWrappers_( pw ), returnWrapper_( rw ) {}
    };
public:
	typedef QList< Method > Methods;
	typedef QMap< QString, Methods > MethodMap;
	LuaContext() {
		L_ = luaL_newstate();
		luaL_openlibs(L_);
		lua_pushlightuserdata( L_, this );
		lua_pushcclosure( L_, &LuaContext::QtConnect , 1);
		lua_setglobal( L_, "qtconnect" );
		dispatcher_.SetLuaContext( this );
	}
	lua_State* LuaState() { return L_; }
	void Eval( const char* code ) {
		ReportErrors( luaL_dostring( L_, code ) );
	}
	//void AddQVariantMap( const QVariantMap&, const char* name = 0 );
	//void AddQVariantList( const QVariantList&, const char* name = 0 );
	//template < typename T > void AddQList( const QList< T >&, const char* name = 0 );
	void AddQObject( QObject* obj, 
		             const char* tableName = 0, //not setting a global name allows to use this method to push a table on the stack
					 const QStringList& methodNames = QStringList(),
					 const QList< QMetaMethod::MethodType >& methodTypes = QList< QMetaMethod::MethodType >()  ) {
        QSet< QString > mn;
		QSet< QMetaMethod::MethodType > mt;
	    foreach( QString s, methodNames ) {
			mn.insert( s );
		}
		foreach( QMetaMethod::MethodType t, methodTypes ) {
			mt.insert( t );
		}
        lua_newtable( L_ );
		const QMetaObject* mo = obj->metaObject();
		for( int i = 0; i != mo->methodCount(); ++i ) {
			QMetaMethod mm = mo->method( i );
			QString name = mm.signature();
			name.truncate( name.indexOf("(") );
			if( !mn.isEmpty() && !mn.contains( name ) ) continue;
			if( !mt.isEmpty() && !mt.contains( mm.methodType() ) ) continue;
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
		if( tableName ) lua_setglobal( L_, tableName );
	}
	~LuaContext() {
		lua_close( L_ );
	}
private:
	static int QtConnect( lua_State* L ) {
		LuaContext& lc = *reinterpret_cast< LuaContext* >( lua_touserdata( L, lua_upvalueindex( 1 ) ) );
		if( lua_gettop( L ) != 3 && lua_gettop( L ) != 4  ) {
			lua_pushstring( L, "qtconnect: Three or four parameters required" );
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
			lua_pushstring( L, "qtconnect: Wrong table format: reference to QObject not found" );
		    lua_error( L );
			return 0;
		}
		QObject* obj = reinterpret_cast< QObject* >( lua_touserdata( L, -1 ) );
		const char* signal = lua_tostring( L, 2 );
		//extract signal arguments info
		const QMetaObject* mo = obj->metaObject();
		
		QString signalSignature = QMetaObject::normalizedSignature( signal );
		const int signalIndex = mo->indexOfSignal( signalSignature.toAscii().constData() );
		if( signalIndex < 0 ) {
			lua_pushstring( L, QString( "Signal '" + signalSignature + "' not found" ).toAscii().constData() );
		    lua_error( L );
			return 0;
		} 
        QMetaMethod mm = mo->method( signalIndex );

		QList< QByteArray > params = mm.parameterTypes();
		QList< QMetaType::Type > types;
		for( QList< QByteArray >::const_iterator i = params.begin();
			 i != params.end(); ++i ) {
				 types.push_back( QMetaType::Type( QMetaType::type( i->constData() ) ) );
		}
		if( !lua_istable( L, 3 ) && !lua_islightuserdata( L, 3 ) ) {
			//push lua callback onto top of stack
			lua_pushvalue( L, 3 );
			const int luaRef = luaL_ref( L, LUA_REGISTRYINDEX );
			const QString slot = QString("_%1_%2_%3").arg( quint64( obj ) ).arg( luaRef ).arg( signalSignature );
			lc.dispatcher_.RegisterSlot( slot, types, luaRef );
			lc.dispatcher_.connectDynamicSlot( obj, signalSignature.toAscii().data(), slot.toAscii().data() ); 
		} else {
			if( lua_islightuserdata( L, 3 ) ) {
				if( lua_gettop( L ) < 4 || !lua_isstring( L, 4 ) ) {
					lua_pushstring( L, "qtconnect: missing target method" );
		            lua_error( L );
					return 0;
				}
			    //fetch QObject* and method/signal signature to invoke in parameter 4
				QObject* targetObj = reinterpret_cast< QObject* >( lua_touserdata( L, 3 ) );
				const char* targetMethod = lua_tostring( L, 4 );
				const QMetaObject* mo = targetObj->metaObject();
				const int targetMethodIdx = mo->indexOfMethod( QMetaObject::normalizedSignature( targetMethod ) ); 
				if( targetMethodIdx < 0 ) {
				    lua_pushstring( L, QString( "Method '" + QString( targetMethod ) + 
						                        "' not found" ).toAscii().constData() );
		            lua_error( L );
					return 0;
				}
				QMetaObject::connect( obj, signalIndex, targetObj, targetMethodIdx );
				return 0;
			} else if( lua_istable( L, 3 ) ) {
				if( lua_gettop( L ) < 4 || !lua_isstring( L, 4 ) ) {
					lua_pushstring( L, "qtconnect: missing target method" );
		            lua_error( L );
					return 0;
				}
				//table wrapping QObject*: extract qobject__ field and signal/slot signature in parameter 4 
				lua_pushstring( L, "qobject__" );
		        lua_gettable( L, 1 );
		        if( lua_isnil( L, -1 ) ) {
			        lua_pushstring( L, "qtconnect: Wrong table format: reference to QObject not found" );
		            lua_error( L );
			        return 0;
				}
				QObject* targetObj = reinterpret_cast< QObject* >( lua_touserdata( L, -1 ) );
				const char* targetMethod = lua_tostring( L, 4 );
				const QMetaObject* mo = targetObj->metaObject();
				const int targetMethodIdx = mo->indexOfMethod( QMetaObject::normalizedSignature( targetMethod ) ); 
				if( targetMethodIdx < 0 ) {
				    lua_pushstring( L, QString( "Method '" + QString( targetMethod ) + 
						                        "' not found" ).toAscii().constData() );
		            lua_error( L );
					return 0;
				}
				QMetaObject::connect( obj, signalIndex, targetObj, targetMethodIdx );
				return 0;
			}
		}
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
//==============================================================================