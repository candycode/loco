cmake_minimum_required(VERSION 2.8)

project(vlwebkit)

#Qt
find_package(Qt4)
include(${QT_USE_FILE})

#VL
set( VL_INCLUDE_DIR "C:/vclibs/Visualization_Library_SDK-2011.02/include" CACHE PATH "VL Include" )
set( VL_SAMPLE_INCLUDE_DIR "C:/vclibs/Visualization_Library_SDK-2011.02/share/examples" CACHE PATH "VL sample Include" )
set( VL_LIB_DIR "C:/vclibs/Visualization_Library_SDK-2011.02/lib" CACHE PATH "VL Lib" )
set( VL_LIBRARIES VLCore VLGraphics VLQt4 FreeType JPG PNG TIFF )

#INCLUDES
include_directories( ${VL_INCLUDE_DIR} ${VL_SAMPLE_INCLUDE_DIR} ${QT_LIBRARY_DIR} ${CMAKE_BINARY_DIR} ) 

#LIB DIR
link_directories( ${VL_LIB_DIR} ${QT_LIBRARY_DIR} )



####SETUP APPLICATIONS
 
# App 1: VL + QWebKit
set( QT_VL_WKIT_MOC_HEADERS VLWindow.h ) #<- Add header files to be processed by moc
QT4_WRAP_CPP( QT_VL_WKIT_MOC_SRCS ${QT_VL_WKIT_MOC_HEADERS} ) #<- generate processed Qt files
#QT4_ADD_RESOURCES( RES_SRCS application.qrc ) #<- resources

set( QT_VL_WKIT_SRCS qtvlwkit.cpp ${QT_VL_WKIT_MOC_SRCS} ) #<- add application source and header files

####CONFIGURE EXECUTABLES
#QT + WKIT + VL
add_executable( qtvlwkit ${QT_VL_WKIT_SRCS} ) #<- executable 1
target_link_libraries( qtvlwkit 
${QT_LIBRARIES} 
QtWebKit4
opengl32
glu32
${QT_QTOPENGL_LIBRARY_RELEASE}
${VL_LIBRARIES} )




