#SRCHEADER
cmake_minimum_required(VERSION 2.8)

set( PROJECTNAME "rtaudio" )
set( PLUGIN_MOC_HEADERS RtAudioPlugin.h )
set( PLUGIN_NAME RtAudio )

project( ${PROJECTNAME} )

#Qt
find_package(Qt4)
include( ${QT_USE_FILE} )
              
include_directories( ${QT_LIBRARY_DIR} ) 
link_directories( ${QT_LIBRARY_DIR}  )

set( MOC_HEADERS ${PLUGIN_MOC_HEADERS} )
QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} ) 

set( SRCS ${MOC_SRCS} RtAudio.cpp RtAudio.h RtError.h RtAudioPlugin.h )

if( WIN32 )
  add_definitions( -D__WINDOWS_DS__ -D_SCL_SECURE_NO_WARNINGS )
  set( SOUNDLIB dsound )
endif()  

add_library( ${PLUGIN_NAME} SHARED ${SRCS} )
set_target_properties( ${PLUGIN_NAME} PROPERTIES PREFIX "" SUFFIX ".qtplugin" )

target_link_libraries( ${PLUGIN_NAME} ${QT_LIBRARIES} ${SOUNDLIB} )


