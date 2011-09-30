cmake_minimum_required(VERSION 2.8)

project("libloco")


set( LINC include )
set( LSRC src )

####  LIB CONFIGURATION
set( LOCO_WKIT "ON" CACHE BOOL "Build with QtWebKit instead of QtScript, requires QtGUI and X(or xvfb) on Linux" )
set( LOCO_GUI  "ON" CACHE BOOL "GUI support, required by WKIT" )
set( LOCO_CONSOLE "ON" CACHE BOOL "stdin/out/err" )
set( LOCO_FSYSTEM "ON" CACHE BOOL "Access to filesystem" )
set( LOCO_SYSTEM "ON" CACHE BOOL "System access to execute programs, required to handle system events." )
set( LOCO_CONTEXT "ON" CACHE BOOL "Adds context to javscript" )
set( LOCO_NETWORK "ON" CACHE BOOL "Access to network" )
option( DYNAMIC "Dynamic library" off )

set( QT_USE_QTNETWORK ON )

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
                     include ) 
link_directories( ${QT_LIBRARY_DIR} )

####SETUP LIB
if( "${LOCO_CONSOLE}" STREQUAL "ON" )
  set( LOCO_CONSOLE_MOC_HEADERS ${LINC}/LocoConsole.h )
endif()
if( "${LOCO_FSYSTEM}" STREQUAL "ON" )
  set( LOCO_FSYSTEM_MOC_HEADERS ${LINC}/LocoFileSystem.h ${LINC}/LocoFile.h ${LINC}/LocoDir.h ${LINC}/LocoFSWatcher.h )
  set( LOCO_FSYSTEM_SRCS ${LSRC}/LocoFileSystem.cpp ${LSRC}/LocoFile.cpp )
  set( LOCO_FSYSTEM_INCLUDE ${LINC}/LocoFileAccessManager.h )
endif()
if( "${LOCO_SYSTEM}" STREQUAL "ON" )
  set( LOCO_SYSTEM_MOC_HEADERS ${LINC}/LocoSystem.h ${LINC}/LocoProcess.h ${LINC}/LocoEventLoop.h )
  set( LOCO_SYSTEM_SRCS ${LSRC}/LocoSystem.cpp ${LSRC}/LocoProcess.cpp )
endif()
if( "${LOCO_WKIT}" STREQUAL "ON" )
  set( LOCO_WKIT_MOC_HEADERS L${LINC}/ocoWebMainWindow.h ${LINC}/LocoWebWindow.h 
       ${LINC}/LocoWebKitJSCore.h ${LINC}/LocoWebKitJSCoreWrapper.h ${LINC}/LocoWebView.h
       ${LINC}/LocoDynamicWebPluginFactory.h )
  set( LOCO_WKIT_HEADERS ${LINC}/LocoWebKitAttributeMap.h ${LINC}/LocoStaticWebPluginFactory.h )
  set( LOCO_WKIT_SRCS ${LSRC}/LocoWebKitAttributeMap.cpp )
endif()
if( "${LOCO_GUI}" STREQUAL "ON" )
  set( LOCO_GUI_MOC_HEADERS ${LINC}/LocoGUI.h )
  set( LOCO_GUI_SRCS ${LSRC}/LocoGUI.cpp )
endif()
if( "${LOCO_SCRIPT}" STREQUAL "ON" )
  set( LOCO_SCRIPT_MOC_HEADERS ${LINC}/LocoDefaultJS.h )
endif()

set( MOC_HEADERS 
  ${LOCO_CONSOLE_MOC_HEADERS}
  ${LINC}/LocoContext.h ${LINC}/EWL.h ${LINC}/LocoObject.h ${LINC}/LocoFilter.h ${LINC}/LocoScriptFilter.h
  ${LOCO_FSYSTEM_MOC_HEADERS} ${LOCO_SYSTEM_MOC_HEADERS} 
  ${LINC}/LocoNetworkAccessManager.h ${LINC}/LocoApp.h ${LINC}/LocoObjectInfo.h ${LINC}/LocoIJSInterpreter.h
  ${LOCO_WKIT_MOC_HEADERS} ${LOCO_GUI_MOC_HEADERS}
  ${LOCO_SCRIPT_MOC_HEADERS} ) 

QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} ) 

###RESOURCES
set( RES res/loco.qrc )
QT4_ADD_RESOURCES( RCC_SRCS ${RES} )
SOURCE_GROUP( Resources REGULAR_EXPRESSION "res/.+" ) 

# Lib 
set( SRCS ${LSRC}/LocoObject.cpp  ${LOCO_FSYSTEM_SRCS} ${LOCO_WKIT_SRCS}
     ${LOCO_SYSTEM_SRCS} ${LOCO_GUI_SRCS} ${LSRC}/LocoContext.cpp )    
    
set( SRCS_INCLUDE ${MOC_HEADERS} ${LOCO_WKIT_HEADERS} ${LOCO_FSYSTEM_INCLUDE} ${LINC}/LocoDefaultJSInit.h ${LINC}/LocoApp.h )

####CONFIGURE LIBRARY
if(DYNAMIC)
add_library( loco SHARED ${SRCS} ${SRCS_INCLUDE} ${MOC_SRCS} ${RCC_SRCS} )
else()
add_library( loco ${SRCS} ${SRCS_INCLUDE} ${MOC_SRCS} ${RCC_SRCS} )
endif()
target_link_libraries( loco ${QT_LIBRARIES} ) 
