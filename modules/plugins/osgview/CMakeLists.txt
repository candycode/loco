#SRCHEADER
cmake_minimum_required(VERSION 2.8)

project( "osgview" )

option( OSG_VIEW_STATIC "Build static library instead dynamic-loadable module" OFF )
set( OSG_INCLUDE_DIR "/usr/local/include" CACHE PATH "OpenSceneGraph include directory" )
set( OSG_LIB_DIR "/use/local/lib64" CACHE PATH "OpenSceneGraph lib directory" )

set( QT_USE_QTOPENGL ON )
set( QT_USE_QTWEBKIT ON )

find_package(Qt4)
include( ${QT_USE_FILE} )

              
include_directories( ${OSG_INCLUDE_DIR} ${LOCO_INCLUDE_DIR} ) 
link_directories( ${QT_LIBRARY_DIR} ${OSG_LIB_DIR} )

set( OSG_LIBRARIES OpenThreads osgDB osgGA osgManipulator osg osgViewer )

set( MOC_HEADERS OSGViewPlugin.h )
QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} ) 

set( SRCS ${MOC_SRCS} QOSGGraphicsScene.h QOSGGraphicsScene.cpp OSGViewPlugin.h )

if( OSG_VIEW_STATIC )
  add_library( osgview ${SRCS} )
else()
  add_library( osgview SHARED ${SRCS} )
endif()

if( MSVC )
  set( GL_LIBRARIES opengl32 glu32 )
else()
  set( GL_LIBRARIES GL GLU )
endif()    
target_link_libraries( osgview ${OSG_LIBRARIES} ${QT_LIBRARIES} ${GL_LIBRARIES} )


