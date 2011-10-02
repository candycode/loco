cmake_minimum_required(VERSION 2.8)

project("sample plugin")

set( QT_DONT_USE_QTGUI ON )

####Qt
find_package(Qt4)
include(${QT_USE_FILE})
               
include_directories( ${QT_LIBRARY_DIR} 
                     ${LOCO_INCLUDE_DIR} ) 
link_directories( ${QT_LIBRARY_DIR} ${LOCO_LIB_DIR} )

####Plugin
set( MOC_HEADERS LocoSamplePlugin.h ) 
QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} ) 
set( SRCS ${MOC_SRCS} )

####CONFIGURE EXECUTABLE
add_library( loco_SamplePlugin SHARED ${SRCS} )
target_link_libraries( loco_SamplePlugin ${QT_LIBRARIES} ${LOCO_LIB_NAME} ) 