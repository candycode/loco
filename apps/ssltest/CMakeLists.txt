cmake_minimum_required(VERSION 2.8)

project("ssltest")

####Qt
find_package(Qt4)

set( QT_USE_QTNETWORK ON )
set( QT_USE_QTWEBKIT ON  )

include(${QT_USE_FILE})
               
include_directories( ${QT_LIBRARY_DIR} ) 
link_directories( ${QT_LIBRARY_DIR} )

set( MOC_HEADERS handler.h )
QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} ) 

# App
set( SRCS ssltest.cpp handler.h )

####CONFIGURE EXECUTABLE
add_executable( ssltest ${SRCS} ${MOC_SRCS} )
target_link_libraries( ssltest ${QT_LIBRARIES} ) 
if(WIN32)
  target_link_libraries( ssltest debug qtmaind optimized qtmain )
endif(WIN32)
