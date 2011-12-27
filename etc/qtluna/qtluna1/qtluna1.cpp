extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <stdexcept>
#include <string>

#include "MyObject.h"



//------------------------------------------------------------------------------

class ParameterWrapper {
public:
	ParameterWrapper( const QString& type ) {
		if( type == "int" ) {
			ac_ = new IntArgConstructor;
		} else if( type == "double" ) {
			ac_ = new DoubleArgConstructor;
		}
	}
    QGenericArgument Arg( lua_State* L ) const {
    	return ac_->Create( L )
    }
    ~ParameterWrapper() { delete ac_; }
private:
    IArgConstructor* ac_;	
};

class ReturnWrapper {
public:
	ReturnWrapper( const QString& type ) {
		if( type == "int" ) {
			rc_ = new ReturnIntConstructor;
		} else if( type == "double" ) {
			rc_ = new ReturnDoubleConstructor;
		}
	}
    void Return( lua_State* L, const QReturnValue& ret ) const {
    	rc_->Return( L, ret );
    }
    ~ReturnConstructor() { delete rc_; }
private:
    IReturnConstructor* rc_;	
};

//------------------------------------------------------------------------------
class LuaContext {
public:
	LuaContext() {
		L_ = luaL_newstate();
		luaL_openlibs(L_);
	}
	void Eval( const char* code ) {
		ReportError( luaL_dostring( L_, code ) );
	}
	void AddQObject( QObject* obj, const char* tableName ) {
        lua_newtable( L_ );
		QMetaObject mo = obj->metaObject();
		for( int m = 0; m != mo.methodCount(); ++m ) {
			QMetaMethod mm = mo.method( i );
			typedef QList< QByteArray > Params;
			Params params = mm.parameterTypes();
			const QString returnType = mm.typeName();
			QString name = mm.signature();
			name.truncate( name.indexOf("(") );
			methods_[ name ].push_back( Method( obj,
			                            m, //index 
			                            GenerateParamWrappers( params, L_ ),
			                            GenerateReturnWrapper( returnType, L_ ) );
			if( method[ name ].size() == 1 ) {
				lua_pushstring( L_, name.toByteArray().constData() );
		        lua_pushlightuserdata( L_, &methods_[ name ] );
		        lua_pushcclosure( L_, LuaContext::InvokeMethod, 1 );
		        lua_rawset( L_, -3 );
		    }	                            
		}
		lua_setglobal( L_, tableName );
		// use lua_gettop to find the number of parameters passed to function
        //add table 
		//iterate over methods and for each name add a
		//c closure taking as the upvalue the 
	}
	~LuaContext() {
		lua_close( L_ );
	}
private:
	static int InvokeMethod( lua_State *L ) {
		QObject* obj = reinterpret_cast< QObject* >( lua_topointer( lua_upvalue( 1 ) ) );
		MethodInfo* mi = reinterpret_cast< MethodInfo* >( lua_topointer( lua_upvalue( 2 ) ) );
		switch( mi->Args() ) {
			case 0: return Invoke0( obj, mi );
			        break;
			case 1: return Invoke1( obj, mi );
			        break;
			case 2: return Invoke2( obj, mi );
			        break;
			case 3: return Invoke3( obj, mi );
			        break;
			case 4: return Invoke4( obj, mi );
			        break;
			case 5: return Invoke5( obj, mi );
			        break;
			case 6: return Invoke6( obj, mi );
			        break;
			default: break;                                                         
		}
	}
    static int Invoke0( obj, mi ) {
    	if( mi->hasReturn() ) {
    		
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
};


//------------------------------------------------------------------------------
int main() {
	LuaContext ctx;
    MyObject myobj;
    ctx.AddQObject( ctx, &myobj, "myobj" );
    ctx.Eval( "myobj.method(\"hello\")" ); 
	return 0;
}