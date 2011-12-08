cmake_minimum_required(VERSION 2.8)

project("loco")

option( LOCOWEBPLUGINS_OSGVIEWER "OSG Viewer web plugin" OFF )
option( LOCOWEBPLUGINS_OSG_GLVIEWER "OSG GL Viewer web plugin" OFF )

if( LOCOWEBPLUGINS_OSGVIEWER )
  add_subdirectory( osg-viewer )
endif()
if( LOCOWEBPLUGINS_OSG_GLVIEWER )  
  add_subdirectory( gl )
endif()  