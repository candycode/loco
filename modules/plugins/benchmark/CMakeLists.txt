#SRCHEADER
cmake_minimum_required(VERSION 2.8)

set( PROJECTNAME "benchmark-plugin" )
set( PLUGIN_MOC_HEADERS BenchmarkPlugin.h )
set( PLUGIN_NAME benchmark )

project( ${PROJECTNAME} )

#ENABLE/DISABLE Qt components
#set( QT_USE_QTOPENGL ON )
#set( QT_USE_QTWEBKIT ON )

#Qt
find_package(Qt4)
include( ${QT_USE_FILE} )
              
include_directories( ${QT_INCLUDE_DIR} ) 
link_directories( ${QT_LIBRARY_DIR}  )

set( MOC_HEADERS ${PLUGIN_MOC_HEADERS} )
QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} ) 

set( SRCS ${MOC_SRCS} )


add_library( ${PLUGIN_NAME} SHARED ${SRCS} )
set_target_properties( ${PLUGIN_NAME} PROPERTIES PREFIX "" SUFFIX ".qtplugin" )

target_link_libraries( ${PLUGIN_NAME} ${QT_LIBRARIES} )


