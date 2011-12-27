cmake_minimum_required(VERSION 2.8)
project(qtluna1scratch)

#set( MOC_HEADERS MyObject.h )
#QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )
include_directories( ${LUA_INCLUDE_DIR} )
link_directories( ${LUA_LIB_DIR} )

add_executable( scratch1 scratch1.cpp )
target_link_libraries( scratch1 ${LUA_LIBRARIES} )

add_executable( scratch2 scratch2.cpp )
target_link_libraries( scratch2 ${LUA_LIBRARIES} )