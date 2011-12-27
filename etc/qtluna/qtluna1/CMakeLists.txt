cmake_minimum_required(VERSION 2.8)
project(qtluna1)

set( MOC_HEADERS MyObject.h )
QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )

add_executable( qtluna1 qtluna1.cpp ) #${MOC_SRCS}  )
target_link_libraries( qtluna1 ${QT_LIBRARIES} ${LUA_LIBRARIES} )