cmake_minimum_required(VERSION 2.8)

project("loco")
set( LOCO_SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "LoCO's root source directory" )
add_subdirectory( libloco )
add_subdirectory( apps )
add_subdirectory( modules )