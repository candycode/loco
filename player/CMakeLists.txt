cmake_minimum_required(VERSION 2.8)

project("loco")

#Qt
find_package(Qt4)
include(${QT_USE_FILE})
                
include_directories( ${QT_LIBRARY_DIR} 
                     ${CMAKE_BINARY_DIR} ) 
link_directories( ${QT_LIBRARY_DIR} )

####SETUP APPLICATION
set( MOC_HEADERS 
  LocoConsole.h LocoContext.h EWL.h LocoObject.h LocoFilter.h
  LocoScriptFilter.h LocoFileSystem.h LocoFile.h LocoDir.h
  LocoSystem.h LocoProcess.h LocoFSWatcher.h LocoNetworkAccessManager.h ) 
QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} ) 

# App
set( SRCS loco.cpp LocoObject.cpp LocoFileSystem.cpp LocoSystem.cpp
     LocoProcess.cpp LocoContext.cpp ) 
set( SRCS_INCLUDE ${MOC_HEADERS} CmdLine.h LocoDefaultJSInit.h LocoApp.h )

####SETUP PLUGIN
set( MOC_HEADERS_PLUGIN EWL.h LocoObject.h plugins/LocoTestPlugin.h ) 
QT4_WRAP_CPP( MOC_SRCS_PLUGIN ${MOC_HEADERS_PLUGIN} )
set( SRCS_PLUGIN LocoObject.cpp LocoObject.h plugins/LocoTestPlugin.h )

####CONFIGURE EXECUTABLES
add_executable( loco ${SRCS} ${SRCS_INCLUDE} ${MOC_SRCS} )

target_link_libraries( loco ${QT_QTWEBKIT_LIBRARY}  ${QT_QTNETWORK_LIBRARY} ${QT_LIBRARIES} ) 
if(WIN32)
  target_link_libraries( loco debug qtmaind optimized qtmain )
endif(WIN32)

####CONFIGURE PLUGINS
add_library( loco_TestPlugin SHARED ${SRCS_PLUGIN} ${MOC_SRCS_PLUGIN} )
target_link_libraries( loco_TestPlugin ${QT_LIBRARIES} )  