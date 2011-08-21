cmake_minimum_required(VERSION 2.8)

project(luaqtgl)
message("$ENV{BOOST_ROOT}")
set( BOOST_ROOT $ENV{BOOST_ROOT} )
####SETUP EXTERNAL LIBRARIES
#Boost
find_package(Boost COMPONENTS thread)

#Qt
find_package(Qt4)
include(${QT_USE_FILE})

#Lua
set( LUA_INCLUDE_DIR "/usr/local/include" CACHE PATH "Lua include directory" )
set( LUA_LIB_DIR "/usr/local/lib" CACHE PATH "Lua lib directory" )
set( LUA_LIBRARIES "Lua51" CACHE FILEPATH "Lua library" )

#Racket scheme - IMPOSSIBLE TO EMBED: IMPOSSIBLE TO CREATE INTERPRETER AND THEN EXECUTE CODE
#                CREATION AND EXECUTION NEED TO HAPPEN IN THE SAME PLACE
set( RACKET_INCLUDE_DIR "/usr/local/include" CACHE PATH "Racket include directory" )
set( RACKET_LIB_DIR "/usr/local/lib" CACHE PATH "Racket lib directory" )
if(WIN32)
set( RACKET_LIBRARY "libracket_89quv2" CACHE FILEPATH "Racket library" )
set( MZCGC_LIBRARY "libmzgc_89quv2" CACHE  FILEPATH "MZ CGC library" )
else()
#set( RACKET_LIBRARY "racket_89quv2" CACHE FILEPATH "Racket library" )
#set( MZCGC_LIBRARY "mzgc_89quv2" CACHE  FILEPATH "MZ CGC library" )
endif()
set( SCHEME_LIBRARIES ${RACKET_LIBRARY} ${MZCGC_LIBRARY} )
#Add all functions form libraries directly in source code;
#libraries are found under the collects/ directory
execute_process( WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                 COMMAND raco ctool --c-mods base.c ++lib scheme/base
                 RESULT_VARIABLE ret_var  )
 
 
#Tinyscheme
set( TINYSCHEME_INCLUDE_DIR "/usr/local/include" CACHE PATH "Tinyscheme include directory" )
set( TINYSCHEME_LIB_DIR "/usr/local/lib" CACHE PATH "Tinyscheme lib directory" )
                 
#OSG
set( OSG_INCLUDE_DIR "/usr/local/include" CACHE PATH "OSG include directory" )
set( OSG_LIB_DIR "/usr/local/lib" CACHE PATH "OSG lib directory" )

include_directories( ${LUA_INCLUDE_DIR} ${OSG_INCLUDE_DIR} ${QT_LIBRARY_DIR} 
                     ${RACKET_INCLUDE_DIR} ${CMAKE_BINARY_DIR} 
                     ${TINYSCHEME_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ) 
link_directories( ${LUA_LIB_DIR} ${OSG_LIB_DIR} ${QT_LIBRARY_DIR} 
                  ${RACKET_LIB_DIR} 
                  ${TINYSCHEME_LIB_DIR} ${BOOST_ROOT}/lib )

####SETUP APPLICATIONS
 
#set( MOC_HEADERS ... ) #<- Add header files to be processed by moc
#QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} ) #<- generate processed Qt files
#QT4_ADD_RESOURCES( RES_SRCS application.qrc ) #<- resources
#source_group( "Resources" application.qrc ) #<- create group for resources

# App 1: Qt + OSG
set( QT_OSG_SRCS osgviewerQtContext-test.cpp ) #<- add application source and header files

# App 2: Qt + OSG with main window, menu and console
set( QT_OSG_CONSOLE_SRCS qtosgconsole.cpp 
                         qtosgconsole-mainwin.cpp
                         qtosgconsole-mainwin.h
                         RacketInterpreter.h
                         RacketInterpreter.cpp
                         osgviewerwidget.h ) #<- add application source and header files
set( QT_OSG_CONSOLE_MOC_HEADERS qtosgconsole-mainwin.h )
QT4_WRAP_CPP( QT_OSG_CONSOLE_MOC_SRCS ${QT_OSG_CONSOLE_MOC_HEADERS} ) 
QT4_ADD_RESOURCES( QT_OSG_CONSOLE_RES qtosgconsole.qrc )
source_group( "Resources" qtosgconsole.qrc )

####CONFIGURE EXECUTABLES
#QT + OSG
add_executable( qtosg ${QT_OSG_SRCS} ) #<- executable 1
target_link_libraries( qtosg 
debug OpenThreadsd optimized OpenThreads
debug osgd optimized osg
debug osgGAd optimized osgGA
debug osgDBd optimized osgDB
debug osgUtild optimized osgUtil
debug osgViewerd optimized osgViewer
debug osgTextd optimized osgText
debug osgQtd optimized osgQt
${QT_LIBRARIES} 
${QT_QTOPENGL_LIBRARY_RELEASE}
${LUA_LIBRARIES} )
if(WIN32)
  target_link_libraries( qtosg debug qtmaind optimized qtmain )
endif(WIN32)  

#QT + OSG W/ MainWindow, menu and console + Lua + Scheme + Tinyscheme
add_executable( qtosgconsole-racket ${QT_OSG_CONSOLE_SRCS} 
                             ${QT_OSG_CONSOLE_MOC_SRCS}
                             ${QT_OSG_CONSOLE_RES} ) #<- executable 2
target_link_libraries( qtosgconsole-racket 
 OpenThreads
 osg
 osgGA
 osgDB
 osgUtil
 osgViewer
 osgText
 osgQt
${QT_LIBRARIES} 
${QT_QTOPENGL_LIBRARY_RELEASE}
${LUA_LIBRARIES}
${SCHEME_LIBRARIES}
 tinyscheme
#C:/vclibs/boost_1_45_0/lib/libboost_thread-vc90-mt-1_45.lib
#C:/vclibs/boost_1_45_0/lib/libboost_date_time-vc90-mt-1_45.lib 
${Boost_LIBRARIES} )
if(WIN32)
  target_link_libraries( qtosgconsole-racket qtmain )
endif(WIN32)  




