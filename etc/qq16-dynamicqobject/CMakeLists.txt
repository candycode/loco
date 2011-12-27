#SRCHEADER
cmake_minimum_required(VERSION 2.8)

project("dynsigslot")


####Qt
find_package(Qt4)
include(${QT_USE_FILE})
               
include_directories( ${QT_LIBRARY_DIR} ) 
link_directories( ${QT_LIBRARY_DIR} )

# App
set( SRCS locoplay.cpp ${RCC_SRCS} )

####CONFIGURE EXECUTABLE
add_executable( locoplay ${SRCS} )
target_link_libraries( locoplay ${QT_LIBRARIES} ${LOCOPLAY_LOCO_LIB} ) 
if(WIN32)
  target_link_libraries( locoplay debug qtmaind optimized qtmain )
endif(WIN32)
