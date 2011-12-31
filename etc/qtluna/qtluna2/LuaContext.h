#pragma once
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <stdexcept>
#include <string>

#include <QMetaMethod>
#include <QString>
#include <QMap>
#include <QList>
#include <QStringList>

#include "LuaCallbackDispatcher.h"

#include "Arguments.h"

namespace qlua {
inline void RaiseLuaError( lua_State* L, const char* errMsg ) {
	lua_pushstring( L, errMsg );
	lua_error( L );
}
inline void RaiseLuaError( lua_State* L, const QString& errMsg ) {
	RaiseLuaError( L, errMsg.toAscii().constData() );
}
inline void RaiseLuaError( lua_State* L, const std::string& errMsg ) {
	RaiseLuaError( L, errMsg.c_str() );
}


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
	enum ObjectDeleteMode { QOBJ_NO_DELETE, QOBJ_IMMEDIATE_DELETE, QOBJ_DELETE_LATER };
	typedef QList< Method > Methods;
	typedef QMap< QObject*, QMap< QString, Methods > > ObjectMethodMap;
	LuaContext() : L_( 0 ), ownQObjects_( false ) {
		L_ = luaL_newstate();
		luaL_openlibs( L_);
		lua_newtable(L_);

		lua_pushstring( L_,  "connect" );
		lua_pushlightuserdata( L_, this );
		lua_pushcclosure( L_, &LuaContext::QtConnect , 1);
		lua_settable( L_, -3 );
		
		lua_pushstring( L_,  "disconnect" );
		lua_pushlightuserdata( L_, this );
		lua_pushcclosure( L_, &LuaContext::QtDisconnect , 1);
		lua_settable( L_, -3 );

		lua_pushstring( L_,  "ownQObjects" );
		lua_pushlightuserdata( L_, this );
		lua_pushcclosure( L_, &LuaContext::SetQObjectsOwnership , 1);
		lua_settable( L_, -3 );

		lua_setglobal( L_, "qlua" );
		dispatcher_.SetLuaContext( this );
		RegisterTypes();
	}
	lua_State* LuaState() const { return L_; }
	void Eval( const char* code ) {
		ReportErrors( luaL_dostring( L_, code ) );
	}
	void AddQVariantMap( const QVariantMap& vm, const char* name = 0 ) {		
        VariantMapToLuaTable( vm, L_ );
	    if( name ) lua_setglobal( L_, name );
	}
	void AddQVariantList( const QVariantList& vl, const char* name = 0 ) {		
        VariantListToLuaTable( vl, L_ );
	    if( name ) lua_setglobal( L_, name );
	}
	void AddQStringList( const QStringList& sl, const char* name = 0 ){		
        StringListToLuaTable( sl, L_ );
	    if( name ) lua_setglobal( L_, name );
	}
	template < typename T > void AddQList( const QList< T >& l, const char* name = 0 ){		
        ListToLuaTable< T >( l, L_ );
	    if( name ) lua_setglobal( L_, name );
	}
	void AddQObject( QObject* obj, 
		             const char* tableName = 0, //not setting a global name allows to use this method to push a table on the stack
					 ObjectDeleteMode deleteMode = QOBJ_NO_DELETE, //destroys QObject instance
					 const QStringList& methodNames = QStringList(),
					 const QList< QMetaMethod::MethodType >& methodTypes = QList< QMetaMethod::MethodType >()  );
	bool OwnQObjects() const { return ownQObjects_; }
	~LuaContext() {
		lua_close( L_ );
	}
private:
	void RemoveObject( QObject* obj ) {
	    objMethods_.remove( obj );
	}
	static int QtConnect( lua_State* L );
	static int QtDisconnect( lua_State* L );
	static int DeleteObject( lua_State* L );
	static int InvokeMethod( lua_State* L );
	static int SetQObjectsOwnership( lua_State* L );
	static int Invoke0( const Method* mi, LuaContext& L );
    static int Invoke1( const Method* mi, LuaContext& L );
	static int Invoke2( const Method* mi, LuaContext& L );
    static int Invoke3( const Method* mi, LuaContext& L );
	static int Invoke4( const Method* mi, LuaContext& L );
    static int Invoke5( const Method* mi, LuaContext& L );
	static int Invoke6( const Method* mi, LuaContext& L );
    static int Invoke7( const Method* mi, LuaContext& L );
	static int Invoke8( const Method* mi, LuaContext& L );
    static int Invoke9( const Method* mi, LuaContext& L );
	static int Invoke10( const Method* mi, LuaContext& L );
    void ReportErrors( int status ) {
    	if( status != 0 ) {
    		std::string err = lua_tostring( L_, -1 );
    		lua_pop( L_, 1 );
    		throw std::runtime_error( err );
    	}
    }
	static void RegisterTypes();
private:
    lua_State* L_;	
	bool ownQObjects_;
	ObjectMethodMap objMethods_;
	LuaCallbackDispatcher dispatcher_; //signal->dispatcher_->Lua callback
};

template < typename T >
T GetValue( const LuaContext& lc, const QString& name ) {
    lua_getglobal( lc.LuaState(), name.toAscii().constData() );
	return luaL_checknumber( lc.LuaState(), -1 );
}

template < typename T >
QList< T > GetValues( const LuaContext& lc, const QString& name ) {
	if( !lua_istable( lc.LuaState(), -1 ) ) throw std::runtime_error( "Not a lua table" );
    return ParseLuaTableAsList< T >( lc.LuaState(), -1 );
}

template <>
inline QString GetValue< QString >( const LuaContext& lc, const QString& name ) {
	lua_getglobal( lc.LuaState(), name.toAscii().constData() );
	return luaL_checkstring( lc.LuaState(), -1 );
}
template <>
inline QVariantMap GetValue< QVariantMap >( const LuaContext& lc, const QString& name ) {
	lua_getglobal( lc.LuaState(), name.toAscii().constData() );
	if( !lua_istable( lc.LuaState(), -1 ) ) throw std::runtime_error( "Not a lua table" );
	return ParseLuaTable( lc.LuaState(), -1 );
}
template <>
inline QVariantList GetValue< QVariantList >( const LuaContext& lc, const QString& name ) {
	lua_getglobal( lc.LuaState(), name.toAscii().constData() );
	if( !lua_istable( lc.LuaState(), -1 ) ) throw std::runtime_error( "Not a lua table" );
	return ParseLuaTableAsVariantList( lc.LuaState(), -1 );
}
template <>
inline QStringList GetValue< QStringList >( const LuaContext& lc, const QString& name ) {
	lua_getglobal( lc.LuaState(), name.toAscii().constData() );
	if( !lua_istable( lc.LuaState(), -1 ) ) throw std::runtime_error( "Not a lua table" );
	return ParseLuaTableAsStringList( lc.LuaState(), -1 );
}

}