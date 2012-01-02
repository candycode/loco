//Who:  Ugo Varetto
//What: Shows how to traverse a lua table in C
//How:  Use 'lua_next' to iterate over <value,key> pairs;
//      'lua_next' replaces the key on top of the stack with
//      the next <value,key> pair
//      1) push nil on stack
//      2) invoke 'next' with index of table until it does not return 0,
//         pop value from stack
//      3) pop nil from stack
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <iostream>
#include <string>


#include <QString>
#include <QVariant>
#include <QVariantMap>
#include <QGenericArgument>


QString LuaKeyToQString( lua_State* L, int idx ) {
	if( lua_isnumber( L, idx ) ) {
		return QString("%1").arg( lua_tointeger( L, idx ) );
	} else if( lua_isstring( L, idx ) ) {
	    return lua_tostring( L, idx );
	} else return "";
}

template < typename ToT, typename FromT >
bool ConvertibleTo( FromT v ) {
    return FromT( ToT( v ) ) == v;
}

QVariant LuaValueToQVariant( lua_State* L, int idx ) {
	if( lua_isboolean( L, idx ) ) {
		return bool( lua_toboolean( L, idx ) );
	} else if( lua_isnumber( L, idx ) ) {
		const double N = lua_tonumber( L, idx );
		if( ConvertibleTo< int >( N ) ) return int( N );
		else if( ConvertibleTo< unsigned int >( N ) ) return (unsigned int)( N );
		else if( ConvertibleTo< long long >( N ) ) return (long long)( N );
		else if( ConvertibleTo< unsigned long long >( N ) ) return (unsigned long long)( N );
		else if( ConvertibleTo< float >( N ) ) return float( N );
		else return N;
	} else if( lua_islightuserdata( L, idx ) ) {
		return lua_topointer( L, idx ); 
	} else if( lua_isstring( L, idx ) ) {
	    return lua_tostring( L, idx );
	} else return QVariant();
}


QVariantMap ParseLuaTable( lua_State* L, bool removeTable = true ) {
    QVariantMap m;
    lua_pushnil(L);  // first key
	const int stackTableIndex = -2;
    while( lua_next( L, stackTableIndex ) != 0 ) {
		/* uses 'key' (at index -2) and 'value' (at index -1) */
		const char* t1 = lua_typename(L, lua_type(L, -2) );
		QString key = LuaKeyToQString( L, -2 );
		const char* t2 = lua_typename(L, lua_type(L, -1) );
		QVariant value = lua_istable( L, -1 ) ? ParseLuaTable( L, false ) : 
			             LuaValueToQVariant( L, -1 );
		m[ key ] = value;
		lua_pop(L, 1);
    }
	if( removeTable ) lua_pop( L, 1 ); // remvove table
	return m;
}

void VariantMapToLuaTable( const QVariantMap& vm, lua_State* L ) {
    lua_newtable( L ); 
	for( QVariantMap::const_iterator i = vm.begin(); i != vm.end(); ++i ) {
		lua_pushstring( L, i.key().toAscii().constData() );
		switch( i.value().type() ) {
			case QVariant::Map: VariantMapToLuaTable( i.value().toMap(), L );
			                    break;
			case QVariant::String: lua_pushstring( L, i.value().toString().toAscii().constData() );
				                   break; 
			case QVariant::Int: lua_pushinteger( L, i.value().toInt() );
	                            break;
            case QVariant::UInt: lua_pushnumber( L, i.value().toUInt() );
	                             break;
            case QVariant::LongLong: lua_pushnumber( L, i.value().toLongLong() );
	                                 break;
            case QVariant::ULongLong: lua_pushnumber( L, i.value().toULongLong() );
	                                  break;
			case QVariant::Bool: lua_pushboolean( L, i.value().toBool() );
	                             break;
			default: break;
		}
		lua_rawset( L, -3 );
	}
}


void PrintVariantMap( const QVariantMap& vm, const QString& padding = "" ) {
	for( QVariantMap::const_iterator i = vm.begin();
		 i != vm.end(); ++i ) {
	    const QString& key = i.key();
		const QVariant& value = i.value();
		std::cout << padding.toStdString();
		switch( value.type() ) {
			case QVariant::Map: PrintVariantMap( value.toMap(), padding + "  " );
			                    break;
			case QVariant::String: std::cout << value.toString().toStdString() << std::endl;
				                   break; 
			case QVariant::Int: std::cout << value.toInt() << std::endl;
	                            break;
            case QVariant::UInt: std::cout << value.toUInt() << std::endl;
	                            break;
            case QVariant::LongLong: std::cout << value.toLongLong() << std::endl;
	                            break;
            case QVariant::ULongLong: std::cout << value.toULongLong() << std::endl;
	                            break;
 			case QVariant::Bool: std::cout << std::boolalpha << value.toBool() << std::endl;
	                            break;
			default: break;
		}
	}
}


//------------------------------------------------------------------------------
int MyCFunction( lua_State* L ) {
	std::cout << "MyCFunction Called" << std::endl; 
	if( !lua_istable( L, -1 ) ) {
		std::cerr << "Not a table" << std::endl;
		return 0;
	}
    PrintVariantMap( ParseLuaTable( L ) );
	QVariantMap vm;
	vm[ "key1" ] = "value1";
	vm[ "key2" ] = true;
	QVariantMap vm2;
	vm2[ "key31" ] = "value31";
	vm[ "key3" ] = vm2;
    VariantMapToLuaTable( vm, L );
	return 1;
}

//------------------------------------------------------------------------------
void ReportErrors( lua_State* L, int status ) {
	if( status != 0 ) {
		std::string err = lua_tostring( L, -1 );
		lua_pop( L, 1 );
		std::cerr << err << std::endl;
	}
}	

//------------------------------------------------------------------------------
int main( int argc, char** argv ) {
	lua_State* L = luaL_newstate();
    luaL_openlibs( L );
   
    lua_pushcfunction( L, &MyCFunction );
    lua_setglobal( L, "myCFunction" );

	std::cout << std::boolalpha; 
	ReportErrors( L, luaL_dostring( L, "print( myCFunction( {1,true,3,table={key='value'} } )['key3']['key31'] )" ) ); 

    lua_close( L );
	QString ciao("ciao");
	QGenericArgument arg = Q_ARG( QString, ciao );
	QGenericReturnArgument rarg = Q_RETURN_ARG( QString, ciao );
	std::cout << (reinterpret_cast< QString* >( arg.data() ) )->toStdString() << std::endl; 
    return 0;	
}
