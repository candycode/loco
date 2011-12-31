cmake_minimum_required(VERSION 2.8)
project(qlua)

include_directories( ${LUA_INCLUDE_DIR} ${QT_INCLUDE_DIR} )
link_directories( ${LUA_LIB_DIR} ${QT_LIBRARY_DIR} )
set( MOC_HEADERS test/TestObject.h )
QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )
add_executable( qluatest test/qlua-test.cpp ${MOC_SRCS} ${MOC_HEADERS} 
                LuaCallbackDispatcher.h 
                LuaCallbackDispatcher.cpp LuaContext.h LuaContext.cpp Arguments.h
                LuaQtTypes.h )
target_link_libraries( qluatest ${QT_LIBRARIES} ${LUA_LIBRARIES} )