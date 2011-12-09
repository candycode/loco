#SRCHEADER
cmake_minimum_required(VERSION 2.8)

set( PROJECTNAME "signal-slot-test" )
set( TARGET_PLUGIN_MOC_HEADERS TargetObjPlugin.h )
set( SOURCE_PLUGIN_MOC_HEADERS SourceObjPlugin.h )
set( TARGET_PLUGIN_NAME target )
set( SOURCE_PLUGIN_NAME source )

project( ${PROJECTNAME} )

#Qt
find_package(Qt4)
include( ${QT_USE_FILE} )
              
include_directories( ${QT_LIBRARY_DIR} ) 
link_directories( ${QT_LIBRARY_DIR}  )

#SOURCE
set( SOURCE_MOC_HEADERS ${SOURCE_PLUGIN_MOC_HEADERS} )
QT4_WRAP_CPP( SOURCE_MOC_SRCS ${SOURCE_MOC_HEADERS} )
set( SOURCE_SRCS ${SOURCE_MOC_SRCS} ) 
add_library( ${SOURCE_PLUGIN_NAME} SHARED ${SOURCE_SRCS} )
set_target_properties( ${SOURCE_PLUGIN_NAME} 
                       PROPERTIES PREFIX "" SUFFIX ".qtplugin" )
target_link_libraries( ${SOURCE_PLUGIN_NAME} ${QT_LIBRARIES} )

#TARGET
set( TARGET_MOC_HEADERS ${TARGET_PLUGIN_MOC_HEADERS} )
QT4_WRAP_CPP( TARGET_MOC_SRCS ${TARGET_MOC_HEADERS} )
set( TARGET_SRCS ${TARGET_MOC_SRCS} ) 
add_library( ${TARGET_PLUGIN_NAME} SHARED ${TARGET_SRCS} )
set_target_properties( ${TARGET_PLUGIN_NAME} 
                       PROPERTIES PREFIX "" SUFFIX ".qtplugin" )
target_link_libraries( ${TARGET_PLUGIN_NAME} ${QT_LIBRARIES} )
