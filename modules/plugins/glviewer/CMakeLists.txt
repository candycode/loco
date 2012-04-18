#SRCHEADER
cmake_minimum_required(VERSION 2.8)

project( "locoviewer" )

set( QT_USE_QTOPENGL ON )

find_package(Qt4)
include( ${QT_USE_FILE} )
              
include_directories( ${QT_INCLUDE_DIR} ) 
link_directories( ${QT_LIBRARY_DIR} )

set( MOC_HEADERS GLViewerPlugin.h openglscene.h)
QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} ) 

set( SRCS ${MOC_SRCS}  model.cpp  model.h  openglscene.cpp  openglscene.h  point3d.h )

add_library( locoviewer SHARED ${SRCS} )

set_target_properties( locoviewer PROPERTIES PREFIX "" SUFFIX ".qtplugin" )
if( MSVC )
  set( GL_LIBRARIES opengl32 glu32 )
else()
  set( GL_LIBRARIES GL GLU )
endif()    
target_link_libraries( locoviewer ${QT_LIBRARIES} ${GL_LIBRARIES} )
