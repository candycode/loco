cmake_minimum_required(VERSION 2.8)
project(qtluna)
#Qt
find_package(Qt4)
include(${QT_USE_FILE})
#Lua
set( LUA_INCLUDE_DIR "/usr/local/include" CACHE PATH "Lua include directory" )
set( LUA_LIB_DIR "/usr/local/lib" CACHE PATH "Lua lib directory" )
set( LUA_LIBRARIES "Lua51" CACHE FILEPATH "Lua library" )

add_subdirectory( scratch )
add_subdirectory( qtluna1 )
add_subdirectory( qtluna2 )