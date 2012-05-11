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

set( PROJECTNAME "signal-slot-test" )
set( TARGET_PLUGIN_MOC_HEADERS TargetObjPlugin.h )
set( SOURCE_PLUGIN_MOC_HEADERS SourceObjPlugin.h )
set( TARGET_PLUGIN_NAME target )
set( SOURCE_PLUGIN_NAME source )

project( ${PROJECTNAME} )

#Qt
find_package(Qt4)
include( ${QT_USE_FILE} )
              
include_directories( ${QT_LIBRARY_DIR} ) 
link_directories( ${QT_LIBRARY_DIR}  )

#SOURCE
set( SOURCE_MOC_HEADERS ${SOURCE_PLUGIN_MOC_HEADERS} )
QT4_WRAP_CPP( SOURCE_MOC_SRCS ${SOURCE_MOC_HEADERS} )
set( SOURCE_SRCS ${SOURCE_MOC_SRCS} ) 
add_library( ${SOURCE_PLUGIN_NAME} SHARED ${SOURCE_SRCS} )
set_target_properties( ${SOURCE_PLUGIN_NAME} 
                       PROPERTIES PREFIX "" SUFFIX ".qtplugin" )
target_link_libraries( ${SOURCE_PLUGIN_NAME} ${QT_LIBRARIES} )

#TARGET
set( TARGET_MOC_HEADERS ${TARGET_PLUGIN_MOC_HEADERS} )
QT4_WRAP_CPP( TARGET_MOC_SRCS ${TARGET_MOC_HEADERS} )
set( TARGET_SRCS ${TARGET_MOC_SRCS} ) 
add_library( ${TARGET_PLUGIN_NAME} SHARED ${TARGET_SRCS} )
set_target_properties( ${TARGET_PLUGIN_NAME} 
                       PROPERTIES PREFIX "" SUFFIX ".qtplugin" )
target_link_libraries( ${TARGET_PLUGIN_NAME} ${QT_LIBRARIES} )
