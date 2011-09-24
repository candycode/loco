cmake_minimum_required(VERSION 2.8)

project("loco")

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
                     ${CMAKE_BINARY_DIR} ) 
link_directories( ${QT_LIBRARY_DIR} )

####SETUP APPLICATION
if( "${LOCO_CONSOLE}" STREQUAL "ON" )
  set( LOCO_CONSOLE_MOC_HEADERS LocoConsole.h )
endif()
if( "${LOCO_FSYSTEM}" STREQUAL "ON" )
  set( LOCO_FSYSTEM_MOC_HEADERS LocoFileSystem.h LocoFile.h LocoDir.h LocoFSWatcher.h )
  set( LOCO_FSYSTEM_SRCS LocoFileSystem.cpp LocoFile.cpp )
  set( LOCO_FSYSTEM_INCLUDE LocoFileAccessManager.h )
endif()
if( "${LOCO_SYSTEM}" STREQUAL "ON" )
  set( LOCO_SYSTEM_MOC_HEADERS LocoSystem.h LocoProcess.h LocoEventLoop.h )
  set( LOCO_SYSTEM_SRCS LocoSystem.cpp LocoProcess.cpp )
endif()
if( "${LOCO_WKIT}" STREQUAL "ON" )
  set( LOCO_WKIT_MOC_HEADERS LocoWebMainWindow.h LocoWebWindow.h 
       LocoWebKitJSCore.h LocoWebKitJSCoreWrapper.h LocoWebView.h )
  set( LOCO_WKIT_HEADERS LocoWebKitAttributeMap.h )
  set( LOCO_WKIT_SRCS LocoWebKitAttributeMap.cpp )
endif()
if( "${LOCO_GUI}" STREQUAL "ON" )
  set( LOCO_GUI_MOC_HEADERS LocoGUI.h )
  set( LOCO_GUI_SRCS LocoGUI.cpp )
endif()
if( "${LOCO_SCRIPT}" STREQUAL "ON" )
  set( LOCO_SCRIPT_MOC_HEADERS LocoDefaultJS.h )
endif()

set( MOC_HEADERS 
  ${LOCO_CONSOLE_MOC_HEADERS}
  LocoContext.h EWL.h LocoObject.h LocoFilter.h LocoScriptFilter.h
  ${LOCO_FSYSTEM_MOC_HEADERS} ${LOCO_SYSTEM_MOC_HEADERS} 
  LocoNetworkAccessManager.h LocoApp.h LocoObjectInfo.h LocoIJSInterpreter.h
  ${LOCO_WKIT_MOC_HEADERS} ${LOCO_GUI_MOC_HEADERS}
  ${LOCO_SCRIPT_MOC_HEADERS} ) 

QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} ) 

###RESOURCES
set( RES res/loco.qrc )
QT4_ADD_RESOURCES( RCC_SRCS ${RES} )
SOURCE_GROUP( Resources REGULAR_EXPRESSION "res/.+" ) 

# App
set( SRCS loco.cpp LocoObject.cpp  ${LOCO_FSYSTEM_SRCS} ${LOCO_WKIT_SRCS}
     ${LOCO_SYSTEM_SRCS} ${LOCO_GUI_SRCS} LocoContext.cpp )    
    
set( SRCS_INCLUDE ${MOC_HEADERS} ${LOCO_WKIT_HEADERS} ${LOCO_FSYSTEM_INCLUDE} LocoDefaultJSInit.h LocoApp.h )

####CONFIGURE EXECUTABLE
add_executable( loco ${SRCS} ${SRCS_INCLUDE} ${MOC_SRCS} ${RCC_SRCS} )
target_link_libraries( loco ${QT_LIBRARIES} ) 
if(WIN32)
  target_link_libraries( loco debug qtmaind optimized qtmain )
endif(WIN32)

################
####SETUP PLUGIN
set( MOC_HEADERS_PLUGIN EWL.h LocoObject.h plugins/LocoTestPlugin.h ) 
QT4_WRAP_CPP( MOC_SRCS_PLUGIN ${MOC_HEADERS_PLUGIN} )
set( SRCS_PLUGIN LocoObject.cpp LocoObject.h plugins/LocoTestPlugin.h )

####CONFIGURE PLUGINS
add_library( loco_TestPlugin SHARED ${SRCS_PLUGIN} ${MOC_SRCS_PLUGIN} )
target_link_libraries( loco_TestPlugin ${QT_LIBRARIES} )  
