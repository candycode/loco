cmake_minimum_required(VERSION 2.8)
project(qtluna1)

include_directories( ${LUA_INCLUDE_DIR} ${QT_INCLUDE_DIR} )
link_directories( ${LUA_LIB_DIR} ${QT_LIBRARY_DIR} )

set( MOC_HEADERS MyObject.h )
QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )

add_executable( qtluna1 qtluna1.cpp ${MOC_SRCS} ${MOC_HEADERS}  )
target_link_libraries( qtluna1 ${QT_LIBRARIES} ${LUA_LIBRARIES} )