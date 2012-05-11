################################################################################
#Copyright (c) 2012, Ugo Varetto
#All rights reserved.
#Redistribution and use in source and binary forms, with or without
#modification, are permitted provided that the following conditions are met:
#    * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#    * Neither the name of the copyright holder nor the
#      names of its contributors may be used to endorse or promote products
#      derived from this software without specific prior written permission.
#
#THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
#ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#DISCLAIMED. IN NO EVENT SHALL UGO VARETTO BE LIABLE FOR ANY
#DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
#(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
#ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
################################################################################

cmake_minimum_required(VERSION 2.8)

project("gl")

option( OSG_WEB_VIEWER_STATIC "Build static library instead dynamic-loadable module" OFF )
set( OSG_INCLUDE_DIR "/usr/local/include" CACHE PATH "OpenSceneGraph include directory" )
set( OSG_LIB_DIR "/use/local/lib64" CACHE PATH "OpenSceneGraph lib directory" )

set( QT_USE_QTOPENGL ON )

find_package(Qt4)
include( ${QT_USE_FILE} )
               
include_directories( ${QT_LIBRARY_DIR} ${OSG_INCLUDE_DIR} ) 
link_directories( ${QT_LIBRARY_DIR} ${OSG_LIB_DIR} )

set( OSG_LIBRARIES OpenThreads osgDB osgGA osgManipulator osgQt osg osgViewer )

set( MOC_HEADERS OSGGLViewerPlugin.h )
QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} ) 

set( SRCS ${MOC_SRCS} )

if( OSG_WEB_VIEWER_STATIC )
  add_library( osgglviewer ${SRCS} )
else()
  add_library( osgglviewer SHARED ${SRCS} )
endif()

target_link_libraries( osgglviewer ${OSG_LIBRARIES} ${QT_LIBRARIES} )


