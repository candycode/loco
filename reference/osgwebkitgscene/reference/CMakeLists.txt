cmake_minimum_required(VERSION 2.8)

project(qtosgwebkitgscene)

#Qt
find_package(Qt4)
include(${QT_USE_FILE})

 
#OSG
set( OSG_INCLUDE_DIR "/usr/local/include" CACHE PATH "OSG include directory" )
set( OSG_LIB_DIR "/usr/local/lib" CACHE PATH "OSG lib directory" )

include_directories( ${OSG_INCLUDE_DIR} ${QT_LIBRARY_DIR} ) 
link_directories( ${OSG_LIB_DIR} ${QT_LIBRARY_DIR}  )

####SETUP APPLICATIONS
 
set( MOC_HEADERS QOSGGraphics.h ) #<- Add header files to be processed by moc
QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} ) #<- generate processed Qt files
#QT4_ADD_RESOURCES( RES_SRCS application.qrc ) #<- resources
#source_group( "Resources" application.qrc ) #<- create group for resources

# App
set( QT_OSG_SRCS main.cpp QOSGGraphics.cpp QOSGGraphics.h ) #<- add application source and header files

####CONFIGURE EXECUTABLE
#QT + OSG
add_executable( qtosgwkit ${QT_OSG_SRCS} ) #<- executable 1
target_link_libraries( qtosgwkit 
debug OpenThreads optimized OpenThreads
debug osg optimized osg
debug osgGA optimized osgGA
debug osgDB optimized osgDB
debug osgUtil optimized osgUtil
debug osgViewer optimized osgViewer
debug osgText optimized osgText
debug osgQt optimized osgQt
QtWebKit4
${QT_LIBRARIES}
opengl32
glu32 
${QT_QTOPENGL_LIBRARY_RELEASE})
if(WIN32)
  target_link_libraries( qtosgwkit debug qtmaind optimized qtmain )
endif(WIN32)  





