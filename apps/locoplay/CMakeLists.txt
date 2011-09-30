cmake_minimum_required(VERSION 2.8)

project("locoplay")

#### APP INFO
set( LOCO_APP_NAME "LoCO" CACHE STRING "Application name" )
set( LOCO_APP_VERSION "1.0.0.1" CACHE STRING "Application version" )
set( LOCO_APP_LICENSE "BSD 3-clause(aka New BSD, Modified BSD)" CACHE STRING "License" )
set( LOCO_APP_AUTHOR "Author" CACHE STRING "Author" )
set( LOCO_APP_COPYRIGHT "Copyright (c) ..." CACHE STRING "Copyright" )
set( LOCO_APP_VENDOR "Vendor" CACHE STRING "Vendor" )
set( LOCO_APP_URL "http://locojs.net" CACHE STRING "URL" )

configure_file( LocoAppConfig.in ${CMAKE_BINARY_DIR}/LocoAppConfig.h )

#### APP CONFIGURATION

set( LOCO_WKIT "ON" CACHE BOOL "Build with QtWebKit instead of QtScript, requires QtGUI and X(or xvfb) on Linux" )
set( LOCO_GUI  "ON" CACHE BOOL "GUI support, required by WKIT" )
set( LOCO_CONSOLE "ON" CACHE BOOL "stdin/out/err" )
set( LOCO_FSYSTEM "ON" CACHE BOOL "Access to filesystem" )
set( LOCO_SYSTEM "ON" CACHE BOOL "System access to execute programs, required to handle system events." )
set( LOCO_CONTEXT "ON" CACHE BOOL "Adds context to javscript" )
set( LOCO_NETWORK "ON" CACHE BOOL "Access to network" )
set( LOCO_INCLUDE_DIR "../../locolib/include" CACHE PATH "LoCO include dir" )
set( LOCO_LIB_DIR "../../locolib/lib" CACHE PATH "LoCO lib dir" )

set( QT_USE_QTNETWORK ON )
set( QT_USE_QTMAIN ON )

if( "${LOCO_GUI}" STREQUAL "OFF" )
  set( QT_DONT_USE_QTGUI "ON" )
else()
  set( QT_DONT_USE_QTGUI "OFF" )
  add_definitions( -DLOCO_GUI ) 
endif()  
if( "${LOCO_WKIT}" STREQUAL "ON" )
  set( QT_DONT_USE_QTGUI OFF )
  set( QT_USE_QTWEBKIT ON )
  set( QT_USE_QTSCRIPT OFF )
  set( LOCO_GUI "ON" )
  add_definitions( -DLOCO_GUI -DLOCO_WKIT ) 
else()
  set( QT_DONT_USE_QTGUI ON )
  set( QT_USE_QTSCRIPT ON )
  set( LOCO_SCRIPT "ON" )
endif()
if( "${LOCO_CONSOLE}" STREQUAL "ON" )
  add_definitions( -DLOCO_CONSOLE ) 
endif()
if( "${LOCO_FSYSTEM}" STREQUAL "ON" )
  add_definitions( -DLOCO_FSYSTEM ) 
endif()
if( "${LOCO_SYSTEM}" STREQUAL "ON" )
  add_definitions( -DLOCO_SYSTEM ) 
endif()
if( "${LOCO_NETWORK}" STREQUAL "ON" )
  add_definitions( -DLOCO_NETWORK ) 
endif()
if( "${LOCO_CONTEXT}" STREQUAL "ON" )
  add_definitions( -DLOCO_CONTEXT ) 
endif()         

####Qt
find_package(Qt4)
include(${QT_USE_FILE})
               
include_directories( ${QT_LIBRARY_DIR} 
                     ${CMAKE_BINARY_DIR}
                     ${LOCO_INCLUDE_DIR} ) 
link_directories( ${QT_LIBRARY_DIR} ${LOCO_LIB_DIR} )

###RESOURCES
set( RES res/loco.qrc )
QT4_ADD_RESOURCES( RCC_SRCS ${RES} )
SOURCE_GROUP( Resources REGULAR_EXPRESSION "res/.+" ) 

# App
set( SRCS loco.cpp )

####CONFIGURE EXECUTABLE
add_executable( loco ${SRCS} )
target_link_libraries( loco ${QT_LIBRARIES} ) 
if(WIN32)
  target_link_libraries( loco debug qtmaind optimized qtmain )
endif(WIN32)
