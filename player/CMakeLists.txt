cmake_minimum_required(VERSION 2.8)

project("loco")

#Qt
find_package(Qt4)
include(${QT_USE_FILE})
                
include_directories( ${QT_LIBRARY_DIR} 
                     ${CMAKE_BINARY_DIR} ) 
link_directories( ${QT_LIBRARY_DIR} )

set( LOCO_WKIT "ON" CACHE BOOL "Build with QtWebKit instead of QtScript, requires QtGUI and X(or xvfb) on Linux" )

####SETUP APPLICATION
set( STD_MOC_HEADERS 
  LocoConsole.h LocoContext.h EWL.h LocoObject.h LocoFilter.h
  LocoScriptFilter.h LocoFileSystem.h LocoFile.h LocoDir.h
  LocoSystem.h LocoProcess.h LocoFSWatcher.h
  LocoNetworkAccessManager.h LocoApp.h LocoObjectInfo.h
  LocoIJSInterpreter.h
  LocoEventLoop.h  ) 

if( "${LOCO_WKIT}" STREQUAL "ON" )  
  set( LOCO_JS_MOC LocoWebWindow.h LocoGUI.h LocoWebKitJSCore.h)
  set( LOCO_JS_SRCS LocoGUI.cpp )
  add_definitions( -DLOCO_GUI -DLOCO_WKIT ) 
else()
  set( LOCO_JS_MOC LocoDefaultJS.h )
endif()  
set( MOC_HEADERS ${STD_MOC_HEADERS} ${LOCO_JS_MOC} )

QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} ) 

###RESOURCES
set( RES res/loco.qrc )
QT4_ADD_RESOURCES( RCC_SRCS ${RES} )
SOURCE_GROUP( Resources REGULAR_EXPRESSION "res/.+" ) 

# App
set( STD_SRCS loco.cpp LocoObject.cpp LocoFileSystem.cpp LocoSystem.cpp
     LocoProcess.cpp LocoContext.cpp LocoFile.cpp ) 
     
set( SRCS ${STD_SRCS} ${LOCO_JS_SRCS} )     
set( SRCS_INCLUDE ${MOC_HEADERS} CmdLine.h LocoFileAccessManager.h LocoDefaultJSInit.h LocoApp.h )

####SETUP PLUGIN
set( MOC_HEADERS_PLUGIN EWL.h LocoObject.h plugins/LocoTestPlugin.h ) 
QT4_WRAP_CPP( MOC_SRCS_PLUGIN ${MOC_HEADERS_PLUGIN} )
set( SRCS_PLUGIN LocoObject.cpp LocoObject.h plugins/LocoTestPlugin.h )

####CONFIGURE EXECUTABLES
add_executable( loco ${SRCS} ${SRCS_INCLUDE} ${MOC_SRCS} ${RCC_SRCS} )

if( "${LOCO_WKIT}" STREQUAL "ON" )    
  set( LOCO_LIBS ${QT_QTWEBKIT_LIBRARY} )
else()
  set( LOCO_LIBS ${QT_QTSCRIPT_LIBRARY} )
endif()

target_link_libraries( loco ${QT_QTNETWORK_LIBRARY} ${QT_LIBRARIES} ${LOCO_LIBS} ) 
  

 
if(WIN32)
  target_link_libraries( loco debug qtmaind optimized qtmain )
endif(WIN32)

####CONFIGURE PLUGINS
add_library( loco_TestPlugin SHARED ${SRCS_PLUGIN} ${MOC_SRCS_PLUGIN} )
target_link_libraries( loco_TestPlugin ${QT_LIBRARIES} )  
