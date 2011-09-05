cmake_minimum_required(VERSION 2.8)

project("loco")

#Qt
find_package(Qt4)
include(${QT_USE_FILE})
                
include_directories( ${QT_LIBRARY_DIR} 
                     ${CMAKE_BINARY_DIR} ) 
link_directories( ${QT_LIBRARY_DIR} )

####SETUP APPLICATION
set( MOC_HEADERS LocoConsole.h LocoContext.h EWL.h ) 
QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} ) #<- generate processed Qt files

# App 1
set( SRCS loco.cpp LocoContext.cpp ) #<- add application source and header files
set( SRCS_INCLUDE LocoConsole.h LocoContext.h EWL.h CmdLine.h )

####SETUP PLUGIN
set( MOC_HEADERS_PLUGIN LocoFSystem.h ) #<- Add header files to be processed by moc
QT4_WRAP_CPP( MOC_SRCS_PLUGIN ${MOC_HEADERS_PLUGIN} ) #<- generate processed Qt files

####CONFIGURE EXECUTABLES
add_executable( loco ${SRCS} ${SRCS_INCLUDE} ${MOC_SRCS} )

target_link_libraries( loco ${QT_QTWEBKIT_LIBRARY}  ${QT_QTNETWORK_LIBRARY} ${QT_LIBRARIES} ) 
if(WIN32)
  target_link_libraries( loco debug qtmaind optimized qtmain )
endif(WIN32)

####CONFIGURE PLUGINS
add_library( loco_FSystem SHARED ${MOC_SRCS_PLUGIN} )
target_link_libraries( loco_FSystem ${QT_LIBRARIES} )  