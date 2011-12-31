#include <QMetaObject>
#include <QSet>
#include <QMetaType>
#include <QMetaProperty>

#include "LuaContext.h"

namespace qlua {
//------------------------------------------------------------------------------
void LuaContext::AddQObject( QObject* obj, 
		                     const char* tableName, 
					         ObjectDeleteMode deleteMode,
							 const QStringList& methodNames,
					         const QList< QMetaMethod::MethodType >& methodTypes ) {
    QSet< QString > mn;
	QSet< QMetaMethod::MethodType > mt;
    foreach( QString s, methodNames ) {
		mn.insert( s );
	}
	foreach( QMetaMethod::MethodType t, methodTypes ) {
		mt.insert( t );
	}
    lua_newtable( L_ );
	//methods
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
		objMethods_[ obj ][ name ].push_back( Method( obj,
		                            mm, 
		                            GenerateParameterWrappers( params ),
		                            GenerateReturnWrapper( returnType ) ) );
		if( objMethods_[ obj ][ name ].size() == 1 ) {
			lua_pushstring( L_, name.toAscii().constData() );
	        lua_pushlightuserdata( L_, &( objMethods_[ obj ][ name ] ) );
			lua_pushlightuserdata( L_, this );
	        lua_pushcclosure( L_, LuaContext::InvokeMethod, 2 );
	        lua_rawset( L_, -3 );
	    }	                            
	}
	lua_pushstring( L_, "qobject__" );
	lua_pushlightuserdata( L_, obj );
    lua_rawset( L_, -3 );
	//properties
	for( int i = 0; i != mo->propertyCount(); ++i ) {
		QMetaProperty mp = mo->property( i );
		lua_pushstring( L_, mp.name() );
		VariantToLuaValue( mp.read( obj ), L_ );
		lua_rawset( L_, -3 );
	}
	if( deleteMode == QOBJ_IMMEDIATE_DELETE ||
		deleteMode == QOBJ_DELETE_LATER ) { //add __gc method
        lua_newtable( L_ ); // push metatable
        lua_pushlightuserdata( L_, obj );
		lua_pushlightuserdata( L_, this );
		lua_pushinteger( L_, int( deleteMode ) );
		lua_pushcclosure( L_, &LuaContext::DeleteObject, 3 ); // push __gc method
        lua_setfield( L_, -2, "__gc" ); // set table['__gc'] = function
        lua_setmetatable( L_, -2 ); // set metatable QObject table
	}

	if( tableName ) lua_setglobal( L_, tableName );
}

//------------------------------------------------------------------------------
int LuaContext::DeleteObject( lua_State* L ) {
    QObject* obj = reinterpret_cast< QObject* >( lua_touserdata( L, lua_upvalueindex( 1 ) ) );
	LuaContext* lc = reinterpret_cast< LuaContext* >( lua_touserdata( L, lua_upvalueindex( 2 ) ) );
	ObjectDeleteMode dm = ObjectDeleteMode( lua_tointeger( L, lua_upvalueindex( 3 ) ) );
	lc->RemoveObject( obj );
	//std::clog << "deleting QObject at " << obj << std::endl;
	if( dm == QOBJ_IMMEDIATE_DELETE ) delete obj;
	else if( dm == QOBJ_DELETE_LATER ) obj->deleteLater();
	return 0;
}

void LuaContext::RegisterTypes() {
    qRegisterMetaType< QList< double > >( QLUA_LIST_FLOAT64 );
	qRegisterMetaType< QList< float > > ( QLUA_LIST_FLOAT32 );
	qRegisterMetaType< QList< int > >   ( QLUA_LIST_INT );
	qRegisterMetaType< QList< short > >( QLUA_LIST_SHORT );
	qRegisterMetaType< QVector< double > >( QLUA_VECTOR_FLOAT64 );
	qRegisterMetaType< QVector< float > >( QLUA_VECTOR_FLOAT32 );
	qRegisterMetaType< QVector< int > >( QLUA_VECTOR_INT );
	qRegisterMetaType< QVector< short > >( QLUA_VECTOR_SHORT );
}

//------------------------------------------------------------------------------
int LuaContext::QtConnect( lua_State* L ) {
	LuaContext& lc = *reinterpret_cast< LuaContext* >( lua_touserdata( L, lua_upvalueindex( 1 ) ) );
	if( lua_gettop( L ) != 3 && lua_gettop( L ) != 4  ) {
		lua_pushstring( L, "qtconnect: Three or four parameters required" );
	    lua_error( L );
		return 0;
	}
	if( !lua_istable( L, 1 ) && !lua_islightuserdata( L, 1 ) ) {
		RaiseLuaError( L, "First parameter to function 'qtconnect' is not a table nor a pointer" );
		return 0;
	}

	QObject* obj = 0;
	if( lua_istable( L, 1 ) ) {
	    lua_pushstring( L, "qobject__" );
	    lua_gettable( L, 1 );
	    if( lua_isnil( L, -1 ) ) {
		    RaiseLuaError( L, "qtconnect: Wrong table format: reference to QObject not found" );
		    return 0;
	    }
		obj = reinterpret_cast< QObject* >( lua_touserdata( L, -1 ) );
	} else obj = reinterpret_cast< QObject* >( lua_touserdata( L, 1 ) );
	
	const char* signal = lua_tostring( L, 2 );
	//extract signal arguments info
	const QMetaObject* mo = obj->metaObject();
	
	QString signalSignature = QMetaObject::normalizedSignature( signal );
	const int signalIndex = mo->indexOfSignal( signalSignature.toAscii().constData() );
	if( signalIndex < 0 ) {
		RaiseLuaError( L, "Signal '" + signalSignature + "' not found" );
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
		lc.dispatcher_.Connect( obj, signalIndex, types, luaRef ); 
	} else {
		if( lua_islightuserdata( L, 3 ) ) {
			if( lua_gettop( L ) < 4 || !lua_isstring( L, 4 ) ) {
				RaiseLuaError( L, "qtconnect: missing target method" );
				return 0;
			}
		    //fetch QObject* and method/signal signature to invoke in parameter 4
			QObject* targetObj = reinterpret_cast< QObject* >( lua_touserdata( L, 3 ) );
			const char* targetMethod = lua_tostring( L, 4 );
			const QMetaObject* mo = targetObj->metaObject();
			const int targetMethodIdx = mo->indexOfMethod( QMetaObject::normalizedSignature( targetMethod ) ); 
			if( targetMethodIdx < 0 ) {
			    RaiseLuaError( L, "Method '" + QString( targetMethod ) + "' not found"  );
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
		        RaiseLuaError( L, "qtconnect: Wrong table format: reference to QObject not found" );
		        return 0;
			}
			QObject* targetObj = reinterpret_cast< QObject* >( lua_touserdata( L, -1 ) );
			const char* targetMethod = lua_tostring( L, 4 );
			const QMetaObject* mo = targetObj->metaObject();
			const int targetMethodIdx = mo->indexOfMethod( QMetaObject::normalizedSignature( targetMethod ) ); 
			if( targetMethodIdx < 0 ) {
			    RaiseLuaError( L, "Method '" + QString( targetMethod ) + "' not found" );
				return 0;
			}
			QMetaObject::connect( obj, signalIndex, targetObj, targetMethodIdx );
			return 0;
		}
	}
    return 0;
}

//------------------------------------------------------------------------------
int LuaContext::InvokeMethod( lua_State *L ) {
	const Methods& m = *( reinterpret_cast< Methods* >( lua_touserdata( L, lua_upvalueindex( 1 ) ) ) );
	LuaContext& lc = *( reinterpret_cast< LuaContext* >( lua_touserdata( L, lua_upvalueindex( 2 ) ) ) );
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
		case  0: return Invoke0( mi, lc );
		         break;
		case  1: return Invoke1( mi, lc );
		         break;
		case  2: return Invoke2( mi, lc );
		        break;
		case  3: return Invoke3( mi, lc );
		        break;
		case  4: return Invoke4( mi, lc );
		         break;
		case  5: return Invoke5( mi, lc );
		         break;
		case  6: return Invoke6( mi, lc );
		         break;
        case  7: return Invoke7( mi, lc );
			     break;
        case  8: return Invoke8( mi, lc );
			     break;
        case  9: return Invoke8( mi, lc );
			     break;
        case 10: return Invoke10( mi, lc );
			     break;
		default: break;                                                         
	}
	return 0;
}

//==============================================================================

void HandleReturnValue( LuaContext& lc, QMetaType::Type type ) {
	if( type == QMetaType::QObjectStar || type == QMetaType::QWidgetStar ) {
		QObject* obj = reinterpret_cast< QObject* >( lua_touserdata( lc.LuaState(), -1 ) );
		lua_pop( lc.LuaState(), 1 );
		lc.AddQObject( obj );
	}
}

int LuaContext::Invoke0( const Method* mi, LuaContext& lc ) {
	bool ok = false;
	lua_State* L = lc.LuaState();
	if( mi->returnWrapper_.Type().isEmpty() ) {
		ok = mi->metaMethod_.invoke( mi->obj_, Qt::DirectConnection );
	    if( ok ) return 0;
	} else {
		ok = mi->metaMethod_.invoke( mi->obj_, Qt::DirectConnection,
    		                         mi->returnWrapper_.Arg() ); //passes the location (void*) where return data will be stored
		if( ok ) {
			mi->returnWrapper_.Push( L );
			HandleReturnValue( lc, mi->returnWrapper_.MetaType() );
		    return 1;
		}
	}
	lua_pushstring( L, "Slot invocation error" );
	lua_error( L );
	return 0;
}
int LuaContext::Invoke1( const Method* mi, LuaContext& lc ) {
	bool ok = false;
	lua_State* L = lc.LuaState();
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
			HandleReturnValue( lc, mi->returnWrapper_.MetaType() );
		    return 1;
		}
	}
	lua_pushstring( L, "Slot invocation error" );
	lua_error( L );
	return 0;
}
int LuaContext::Invoke2( const Method* mi, LuaContext& lc ) {
	bool ok = false;
	lua_State* L = lc.LuaState();
	if( mi->returnWrapper_.Type().isEmpty() ) {
		ok = mi->metaMethod_.invoke( mi->obj_, Qt::DirectConnection,
          			                 mi->paramWrappers_[ 0 ].Arg( L, 1 ),
									 mi->paramWrappers_[ 1 ].Arg( L, 2 ) );
	    if( ok ) return 0;
	} else {
		ok = mi->metaMethod_.invoke( mi->obj_, Qt::DirectConnection,
    		                         mi->returnWrapper_.Arg(), //passes the location (void*) where return data will be stored
          			                 mi->paramWrappers_[ 0 ].Arg( L, 1 ),
									 mi->paramWrappers_[ 1 ].Arg( L, 2 ) );
		if( ok ) {
			mi->returnWrapper_.Push( L );
			HandleReturnValue( lc, mi->returnWrapper_.MetaType() );
		    return 1;
		}
	}
	lua_pushstring( L, "Slot invocation error" );
	lua_error( L );
	return 0;
}
int LuaContext::Invoke3( const Method* mi, LuaContext& lc ) {
	bool ok = false;
	lua_State* L = lc.LuaState();
	if( mi->returnWrapper_.Type().isEmpty() ) {
		ok = mi->metaMethod_.invoke( mi->obj_, Qt::DirectConnection,
          			                 mi->paramWrappers_[ 0 ].Arg( L, 1 ),
									 mi->paramWrappers_[ 1 ].Arg( L, 2 ),
									 mi->paramWrappers_[ 2 ].Arg( L, 3 ) );
	    if( ok ) return 0;
	} else {
		ok = mi->metaMethod_.invoke( mi->obj_, Qt::DirectConnection,
    		                         mi->returnWrapper_.Arg(), //passes the location (void*) where return data will be stored
          			                 mi->paramWrappers_[ 0 ].Arg( L, 1 ),
									 mi->paramWrappers_[ 1 ].Arg( L, 2 ),
									 mi->paramWrappers_[ 2 ].Arg( L, 3 ) );
		if( ok ) {
			mi->returnWrapper_.Push( L );
			HandleReturnValue( lc, mi->returnWrapper_.MetaType() );
		    return 1;
		}
	}
	lua_pushstring( L, "Slot invocation error" );
	lua_error( L );
	return 0;
}
int LuaContext::Invoke4( const Method* mi, LuaContext& lc ) {
	bool ok = false;
	lua_State* L = lc.LuaState();
	if( mi->returnWrapper_.Type().isEmpty() ) {
		ok = mi->metaMethod_.invoke( mi->obj_, Qt::DirectConnection,
          			                 mi->paramWrappers_[ 0 ].Arg( L, 1 ),
									 mi->paramWrappers_[ 1 ].Arg( L, 2 ),
									 mi->paramWrappers_[ 2 ].Arg( L, 3 ),
									 mi->paramWrappers_[ 3 ].Arg( L, 4 ) );
	    if( ok ) return 0;
	} else {
		ok = mi->metaMethod_.invoke( mi->obj_, Qt::DirectConnection,
    		                         mi->returnWrapper_.Arg(), //passes the location (void*) where return data will be stored
          			                 mi->paramWrappers_[ 0 ].Arg( L, 1 ),
									 mi->paramWrappers_[ 1 ].Arg( L, 2 ),
									 mi->paramWrappers_[ 2 ].Arg( L, 3 ),
									 mi->paramWrappers_[ 3 ].Arg( L, 4 ) );
		if( ok ) {
			mi->returnWrapper_.Push( L );
			HandleReturnValue( lc, mi->returnWrapper_.MetaType() );
		    return 1;
		}
	}
	lua_pushstring( L, "Slot invocation error" );
	lua_error( L );
	return 0;
}
int LuaContext::Invoke5( const Method* mi, LuaContext& lc ) {
	bool ok = false;
	lua_State* L = lc.LuaState();
	if( mi->returnWrapper_.Type().isEmpty() ) {
		ok = mi->metaMethod_.invoke( mi->obj_, Qt::DirectConnection,
          			                 mi->paramWrappers_[ 0 ].Arg( L, 1 ),
									 mi->paramWrappers_[ 1 ].Arg( L, 2 ),
									 mi->paramWrappers_[ 2 ].Arg( L, 3 ),
									 mi->paramWrappers_[ 3 ].Arg( L, 4 ),
									 mi->paramWrappers_[ 4 ].Arg( L, 5 ) );
	    if( ok ) return 0;
	} else {
		ok = mi->metaMethod_.invoke( mi->obj_, Qt::DirectConnection,
    		                         mi->returnWrapper_.Arg(), //passes the location (void*) where return data will be stored
          			                 mi->paramWrappers_[ 0 ].Arg( L, 1 ),
									 mi->paramWrappers_[ 1 ].Arg( L, 2 ),
									 mi->paramWrappers_[ 2 ].Arg( L, 3 ),
									 mi->paramWrappers_[ 3 ].Arg( L, 4 ),
									 mi->paramWrappers_[ 4 ].Arg( L, 5 ) );
		if( ok ) {
			mi->returnWrapper_.Push( L );
			HandleReturnValue( lc, mi->returnWrapper_.MetaType() );
		    return 1;
		}
	}
	lua_pushstring( L, "Slot invocation error" );
	lua_error( L );
	return 0;
}
int LuaContext::Invoke6( const Method* mi, LuaContext& lc ) {
	bool ok = false;
	lua_State* L = lc.LuaState();
	if( mi->returnWrapper_.Type().isEmpty() ) {
		ok = mi->metaMethod_.invoke( mi->obj_, Qt::DirectConnection,
          			                 mi->paramWrappers_[ 0 ].Arg( L, 1 ),
									 mi->paramWrappers_[ 1 ].Arg( L, 2 ),
									 mi->paramWrappers_[ 2 ].Arg( L, 3 ),
									 mi->paramWrappers_[ 3 ].Arg( L, 4 ),
									 mi->paramWrappers_[ 4 ].Arg( L, 5 ),
									 mi->paramWrappers_[ 5 ].Arg( L, 6 ) );
	    if( ok ) return 0;
	} else {
		ok = mi->metaMethod_.invoke( mi->obj_, Qt::DirectConnection,
    		                         mi->returnWrapper_.Arg(), //passes the location (void*) where return data will be stored
          			                 mi->paramWrappers_[ 0 ].Arg( L, 1 ),
									 mi->paramWrappers_[ 1 ].Arg( L, 2 ),
									 mi->paramWrappers_[ 2 ].Arg( L, 3 ),
									 mi->paramWrappers_[ 3 ].Arg( L, 4 ),
									 mi->paramWrappers_[ 4 ].Arg( L, 5 ),
									 mi->paramWrappers_[ 5 ].Arg( L, 6 ) );
		if( ok ) {
			mi->returnWrapper_.Push( L );
			HandleReturnValue( lc, mi->returnWrapper_.MetaType() );
		    return 1;
		}
	}
	lua_pushstring( L, "Slot invocation error" );
	lua_error( L );
	return 0;
}
int LuaContext::Invoke7( const Method* mi, LuaContext& lc ) {
	bool ok = false;
	lua_State* L = lc.LuaState();
	if( mi->returnWrapper_.Type().isEmpty() ) {
		ok = mi->metaMethod_.invoke( mi->obj_, Qt::DirectConnection,
          			                 mi->paramWrappers_[ 0 ].Arg( L, 1 ),
									 mi->paramWrappers_[ 1 ].Arg( L, 2 ),
									 mi->paramWrappers_[ 2 ].Arg( L, 3 ),
									 mi->paramWrappers_[ 3 ].Arg( L, 4 ),
									 mi->paramWrappers_[ 4 ].Arg( L, 5 ),
									 mi->paramWrappers_[ 5 ].Arg( L, 6 ),
									 mi->paramWrappers_[ 6 ].Arg( L, 7 ) );
	    if( ok ) return 0;
	} else {
		ok = mi->metaMethod_.invoke( mi->obj_, Qt::DirectConnection,
    		                         mi->returnWrapper_.Arg(), //passes the location (void*) where return data will be stored
          			                 mi->paramWrappers_[ 0 ].Arg( L, 1 ),
									 mi->paramWrappers_[ 1 ].Arg( L, 2 ),
									 mi->paramWrappers_[ 2 ].Arg( L, 3 ),
									 mi->paramWrappers_[ 3 ].Arg( L, 4 ),
									 mi->paramWrappers_[ 4 ].Arg( L, 5 ),
									 mi->paramWrappers_[ 5 ].Arg( L, 6 ),
									 mi->paramWrappers_[ 6 ].Arg( L, 7 ) );
		if( ok ) {
			mi->returnWrapper_.Push( L );
			HandleReturnValue( lc, mi->returnWrapper_.MetaType() );
		    return 1;
		}
	}
	lua_pushstring( L, "Slot invocation error" );
	lua_error( L );
	return 0;
}
int LuaContext::Invoke8( const Method* mi, LuaContext& lc ) {
	bool ok = false;
	lua_State* L = lc.LuaState();
	if( mi->returnWrapper_.Type().isEmpty() ) {
		ok = mi->metaMethod_.invoke( mi->obj_, Qt::DirectConnection,
          			                 mi->paramWrappers_[ 0 ].Arg( L, 1 ),
									 mi->paramWrappers_[ 1 ].Arg( L, 2 ),
									 mi->paramWrappers_[ 2 ].Arg( L, 3 ),
									 mi->paramWrappers_[ 3 ].Arg( L, 4 ),
									 mi->paramWrappers_[ 4 ].Arg( L, 5 ),
									 mi->paramWrappers_[ 5 ].Arg( L, 6 ),
									 mi->paramWrappers_[ 6 ].Arg( L, 7 ),
									 mi->paramWrappers_[ 7 ].Arg( L, 8 ) );
	    if( ok ) return 0;
	} else {
		ok = mi->metaMethod_.invoke( mi->obj_, Qt::DirectConnection,
    		                         mi->returnWrapper_.Arg(), //passes the location (void*) where return data will be stored
          			                 mi->paramWrappers_[ 0 ].Arg( L, 1 ),
									 mi->paramWrappers_[ 1 ].Arg( L, 2 ),
									 mi->paramWrappers_[ 2 ].Arg( L, 3 ),
									 mi->paramWrappers_[ 3 ].Arg( L, 4 ),
									 mi->paramWrappers_[ 4 ].Arg( L, 5 ),
									 mi->paramWrappers_[ 5 ].Arg( L, 6 ),
									 mi->paramWrappers_[ 6 ].Arg( L, 7 ),
									 mi->paramWrappers_[ 7 ].Arg( L, 8 ) );
		if( ok ) {
			mi->returnWrapper_.Push( L );
			HandleReturnValue( lc, mi->returnWrapper_.MetaType() );
		    return 1;
		}
	}
	lua_pushstring( L, "Slot invocation error" );
	lua_error( L );
	return 0;
}
int LuaContext::Invoke9( const Method* mi, LuaContext& lc ) {
	bool ok = false;
	lua_State* L = lc.LuaState();
	if( mi->returnWrapper_.Type().isEmpty() ) {
		ok = mi->metaMethod_.invoke( mi->obj_, Qt::DirectConnection,
          			                 mi->paramWrappers_[ 0 ].Arg( L, 1 ),
									 mi->paramWrappers_[ 1 ].Arg( L, 2 ),
									 mi->paramWrappers_[ 2 ].Arg( L, 3 ),
									 mi->paramWrappers_[ 3 ].Arg( L, 4 ),
									 mi->paramWrappers_[ 4 ].Arg( L, 5 ),
									 mi->paramWrappers_[ 5 ].Arg( L, 6 ),
									 mi->paramWrappers_[ 6 ].Arg( L, 7 ),
									 mi->paramWrappers_[ 7 ].Arg( L, 8 ),
									 mi->paramWrappers_[ 8 ].Arg( L, 9 ) );
	    if( ok ) return 0;
	} else {
		ok = mi->metaMethod_.invoke( mi->obj_, Qt::DirectConnection,
    		                         mi->returnWrapper_.Arg(), //passes the location (void*) where return data will be stored
          			                 mi->paramWrappers_[ 0 ].Arg( L, 1 ),
									 mi->paramWrappers_[ 1 ].Arg( L, 2 ),
									 mi->paramWrappers_[ 2 ].Arg( L, 3 ),
									 mi->paramWrappers_[ 3 ].Arg( L, 4 ),
									 mi->paramWrappers_[ 4 ].Arg( L, 5 ),
									 mi->paramWrappers_[ 5 ].Arg( L, 6 ),
									 mi->paramWrappers_[ 6 ].Arg( L, 7 ),
									 mi->paramWrappers_[ 7 ].Arg( L, 8 ),
									 mi->paramWrappers_[ 8 ].Arg( L, 9 ) );
		if( ok ) {
			mi->returnWrapper_.Push( L );
			HandleReturnValue( lc, mi->returnWrapper_.MetaType() );
		    return 1;
		}
	}
	lua_pushstring( L, "Slot invocation error" );
	lua_error( L );
	return 0;
}
int LuaContext::Invoke10( const Method* mi, LuaContext& lc ) {
	bool ok = false;
	lua_State* L = lc.LuaState();
	if( mi->returnWrapper_.Type().isEmpty() ) {
		ok = mi->metaMethod_.invoke( mi->obj_, Qt::DirectConnection,
          			                 mi->paramWrappers_[ 0 ].Arg( L,  1 ),
									 mi->paramWrappers_[ 1 ].Arg( L,  2 ),
									 mi->paramWrappers_[ 2 ].Arg( L,  3 ),
									 mi->paramWrappers_[ 3 ].Arg( L,  4 ),
									 mi->paramWrappers_[ 4 ].Arg( L,  5 ),
									 mi->paramWrappers_[ 5 ].Arg( L,  6 ),
									 mi->paramWrappers_[ 6 ].Arg( L,  7 ),
									 mi->paramWrappers_[ 7 ].Arg( L,  8 ),
									 mi->paramWrappers_[ 8 ].Arg( L,  9 ),
									 mi->paramWrappers_[ 9 ].Arg( L, 10 ) );
	    if( ok ) return 0;
	} else {
		ok = mi->metaMethod_.invoke( mi->obj_, Qt::DirectConnection,
    		                         mi->returnWrapper_.Arg(), //passes the location (void*) where return data will be stored
          			                 mi->paramWrappers_[ 0 ].Arg( L,  1 ),
									 mi->paramWrappers_[ 1 ].Arg( L,  2 ),
									 mi->paramWrappers_[ 2 ].Arg( L,  3 ),
									 mi->paramWrappers_[ 3 ].Arg( L,  4 ),
									 mi->paramWrappers_[ 4 ].Arg( L,  5 ),
									 mi->paramWrappers_[ 5 ].Arg( L,  6 ),
									 mi->paramWrappers_[ 6 ].Arg( L,  7 ),
									 mi->paramWrappers_[ 7 ].Arg( L,  8 ),
									 mi->paramWrappers_[ 8 ].Arg( L,  9 ),
									 mi->paramWrappers_[ 9 ].Arg( L, 10 ) );
		if( ok ) {
			mi->returnWrapper_.Push( L );
			HandleReturnValue( lc, mi->returnWrapper_.MetaType() );
		    return 1;
		}
	}
	lua_pushstring( L, "Slot invocation error" );
	lua_error( L );
	return 0;
}
}