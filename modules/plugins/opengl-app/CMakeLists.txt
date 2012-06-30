#SRCHEADER
cmake_minimum_required(VERSION 2.8)

set( PROJECTNAME "opengl-app" )
set( PLUGIN_MOC_HEADERS GLApp.h )
set( PLUGIN_NAME "opengl-app" )

project( ${PROJECTNAME} )

find_package(OpenGL)

#ENABLE/DISABLE Qt components
set( QT_USE_QTOPENGL ON )
#set( QT_USE_QTWEBKIT ON )

#Qt
find_package(Qt4)
include( ${QT_USE_FILE} )
              
include_directories( ${QT_LIBRARY_DIR} ) 
link_directories( ${QT_LIBRARY_DIR}  )

set( MOC_HEADERS ${PLUGIN_MOC_HEADERS} )
QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} ) 

set( SRCS ${MOC_SRCS} )


add_library( ${PLUGIN_NAME} SHARED ${SRCS} )

target_link_libraries( ${PLUGIN_NAME} ${QT_LIBRARIES} ${OPENGL_LIBRARIES} GLEW )

set_target_properties( ${PLUGIN_NAME} 
                       PROPERTIES PREFIX "" SUFFIX ".qtplugin" )
