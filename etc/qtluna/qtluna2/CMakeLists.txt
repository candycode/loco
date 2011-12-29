cmake_minimum_required(VERSION 2.8)
project(qlua)

include_directories( ${LUA_INCLUDE_DIR} ${QT_INCLUDE_DIR} )
link_directories( ${LUA_LIB_DIR} ${QT_LIBRARY_DIR} )
set( MOC_HEADERS MyObject.h )
QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )
add_executable( qlua1 qlua1.cpp ${MOC_SRCS} ${MOC_HEADERS} 
                DynamicLuaQObject.h 
                DynamicLuaQObject.cpp LuaContext.h LuaContext.cpp Arguments.h
                LuaQtTypes.h )
target_link_libraries( qlua1 ${QT_LIBRARIES} ${LUA_LIBRARIES} )