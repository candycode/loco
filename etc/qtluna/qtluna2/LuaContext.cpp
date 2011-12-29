#include "LuaContext.h"

namespace qlua {
//------------------------------------------------------------------------------
void LuaContext::AddQObject( QObject* obj, 
		             const char* tableName, 
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

	if( tableName ) lua_setglobal( L_, tableName );
}

//------------------------------------------------------------------------------
int LuaContext::QtConnect( lua_State* L ) {
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
		lc.dispatcher_.Connect( obj, signalSignature.toAscii().data(), slot.toAscii().data() ); 
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

//------------------------------------------------------------------------------
int LuaContext::InvokeMethod( lua_State *L ) {
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
		case  0: return Invoke0( mi, L );
		         break;
		case  1: return Invoke1( mi, L );
		         break;
		case  2: return Invoke2( mi, L );
		        break;
		case  3: return Invoke3( mi, L );
		        break;
		case  4: return Invoke4( mi, L );
		         break;
		case  5: return Invoke5( mi, L );
		         break;
		case  6: return Invoke6( mi, L );
		         break;
        case  7: return Invoke7( mi, L );
			     break;
        case  8: return Invoke8( mi, L );
			     break;
        case  9: return Invoke8( mi, L );
			     break;
        case 10: return Invoke10( mi, L );
			     break;
		default: break;                                                         
	}
	return 0;
}

//==============================================================================
int LuaContext::Invoke0( const Method* mi, lua_State* L ) {
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
int LuaContext::Invoke1( const Method* mi, lua_State* L ) {
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
int LuaContext::Invoke2( const Method* mi, lua_State* L ) {
	bool ok = false;
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
		    return 1;
		}
	}
	lua_pushstring( L, "Slot invocation error" );
	lua_error( L );
	return 0;
}
int LuaContext::Invoke3( const Method* mi, lua_State* L ) {
	bool ok = false;
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
		    return 1;
		}
	}
	lua_pushstring( L, "Slot invocation error" );
	lua_error( L );
	return 0;
}
int LuaContext::Invoke4( const Method* mi, lua_State* L ) {
	bool ok = false;
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
		    return 1;
		}
	}
	lua_pushstring( L, "Slot invocation error" );
	lua_error( L );
	return 0;
}
int LuaContext::Invoke5( const Method* mi, lua_State* L ) {
	bool ok = false;
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
		    return 1;
		}
	}
	lua_pushstring( L, "Slot invocation error" );
	lua_error( L );
	return 0;
}
int LuaContext::Invoke6( const Method* mi, lua_State* L ) {
	bool ok = false;
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
		    return 1;
		}
	}
	lua_pushstring( L, "Slot invocation error" );
	lua_error( L );
	return 0;
}
int LuaContext::Invoke7( const Method* mi, lua_State* L ) {
	bool ok = false;
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
		    return 1;
		}
	}
	lua_pushstring( L, "Slot invocation error" );
	lua_error( L );
	return 0;
}
int LuaContext::Invoke8( const Method* mi, lua_State* L ) {
	bool ok = false;
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
		    return 1;
		}
	}
	lua_pushstring( L, "Slot invocation error" );
	lua_error( L );
	return 0;
}
int LuaContext::Invoke9( const Method* mi, lua_State* L ) {
	bool ok = false;
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
		    return 1;
		}
	}
	lua_pushstring( L, "Slot invocation error" );
	lua_error( L );
	return 0;
}
int LuaContext::Invoke10( const Method* mi, lua_State* L ) {
	bool ok = false;
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
		    return 1;
		}
	}
	lua_pushstring( L, "Slot invocation error" );
	lua_error( L );
	return 0;
}
}