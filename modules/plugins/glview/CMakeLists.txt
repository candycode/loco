#SRCHEADER
cmake_minimum_required(VERSION 2.8)

project( "glview" )

set( QT_USE_QTOPENGL ON )
set( QT_USE_QTWEBKIT ON )

find_package(Qt4)
include( ${QT_USE_FILE} )

link_directories( ${QT_LIBRARY_DIR} )

set( MOC_HEADERS GLViewPlugin.h openglscene.h )
QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} ) 

set( SRCS ${MOC_SRCS} GLViewPlugin.h model.cpp openglscene.cpp)

add_library( glview SHARED ${SRCS} )

if( MSVC )
  set( GL_LIBRARIES opengl32 glu32 )
else()
  set( GL_LIBRARIES GL GLU )
endif()    
target_link_libraries( glview ${QT_LIBRARIES} ${GL_LIBRARIES} )


