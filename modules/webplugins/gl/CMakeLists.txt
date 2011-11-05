#SRCHEADER
cmake_minimum_required(VERSION 2.8)

project("gl")

option( OSG_WEB_VIEWER_STATIC "Build static library instead dynamic-loadable module" OFF )
set( OSG_INCLUDE_DIR "/usr/local/include" CACHE PATH "OpenSceneGraph include directory" )
set( OSG_LIB_DIR "/use/local/lib64" CACHE PATH "OpenSceneGraph lib directory" )

set( QT_USE_QTOPENGL ON )

find_package(Qt4)
include( ${QT_USE_FILE} )
               
include_directories( ${QT_LIBRARY_DIR} ${OSG_INCLUDE_DIR} ) 
link_directories( ${QT_LIBRARY_DIR} ${OSG_LIB_DIR} )

set( OSG_LIBRARIES OpenThreads osgDB osgGA osgManipulator osgQt osg osgViewer )

set( MOC_HEADERS OSGGLViewerPlugin.h )
QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} ) 

set( SRCS ${MOC_SRCS} )

if( OSG_WEB_VIEWER_STATIC )
  add_library( osgglviewer ${SRCS} )
else()
  add_library( osgglviewer SHARED ${SRCS} )
endif()

target_link_libraries( osgglviewer ${OSG_LIBRARIES} ${QT_LIBRARIES} )


