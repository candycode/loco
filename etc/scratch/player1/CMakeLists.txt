cmake_minimum_required(VERSION 2.8)

project("locoplay1")

#Qt
find_package(Qt4)
include(${QT_USE_FILE})
                
include_directories( ${QT_LIBRARY_DIR} 
                     ${CMAKE_BINARY_DIR} ) 
link_directories( ${QT_LIBRARY_DIR} )

####SETUP APPLICATIONS
#LocoLoader.h 
set( MOC_HEADERS LocoStdout.h LocoWebView.h JEnvConfig.h ) #<- Add header files to be processed by moc
QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} ) #<- generate processed Qt files

# App 1
set( SRCS locoplay1.cpp ) #<- add application source and header files
set( SRCS_INCLUDE LocoStdout.h LocoWebView.h LocoIWebConfig.h JEnvConfig.h )

####CONFIGURE EXECUTABLES
add_executable( locoplay1 ${SRCS} ${SRCS_INCLUDE} ${MOC_SRCS} )

target_link_libraries( locoplay1 QtWebkit4 QtNetwork4 ${QT_LIBRARIES} ) 
if(WIN32)
  target_link_libraries( locoplay1 debug qtmaind optimized qtmain )
endif(WIN32)  





