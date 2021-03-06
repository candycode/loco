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

project("locoplay")

#### APP INFO
set( LOCOPLAY_APP_NAME "LoCO Player" CACHE STRING "Application name" )
set( LOCOPLAY_APP_VERSION "1.0.0.0" CACHE STRING "Application version" )
set( LOCOPLAY_APP_LICENSE "BSD 3-clause(aka New BSD, Modified BSD)" CACHE STRING "License" )
set( LOCOPLAY_APP_AUTHOR "UV" CACHE STRING "UV" )
set( LOCOPLAY_APP_COPYRIGHT "Copyright (c) 2011 Ugo Varetto" CACHE STRING "Copyright" )
set( LOCOPLAY_APP_VENDOR "LoCO" CACHE STRING "Vendor" )
set( LOCOPLAY_APP_URL "http://locojs.net" CACHE STRING "URL" )

configure_file( LocoAppConfig.in ${CMAKE_BINARY_DIR}/LocoAppConfig.h )

#### APP CONFIGURATION

option( LOCOPLAY_WKIT "Build with QtWebKit instead of QtScript, requires QtGUI and X(or xvfb) on Linux" ON )
option( LOCOPLAY_GUI  "GUI support, required by WKIT" ON )
option( LOCOPLAY_CONSOLE "stdin/out/err" ON )
option( LOCOPLAY_FSYSTEM "Access to filesystem" ON )
option( LOCOPLAY_SYSTEM "System access to programs, required to handle system events." ON )
option( LOCOPLAY_CONTEXT "Adds context to javscript" ON )
option( LOCOPLAY_NETWORK "Access to network" ON )
option( LOCOPLAY_JS_SYNTAX_CHECKER "Add Javascript syntax checker" ON )
set( LOCO_INCLUDE_DIR "${LOCO_SRC_ROOT}/libloco/include" CACHE PATH "LoCO include dir" )
set( LOCO_LIB_DIR "${CMAKE_BINARY_DIR}/libloco" CACHE PATH "LoCO lib dir" )
if( LOCOPLAY_WKIT )
  set( LOCOPLAY_LOCO_LIB "loco-wkit" CACHE STRING "LoCO library name" )
else()
  set( LOCOPLAY_LOCO_LIB "loco" CACHE STRING "LoCO library name" )
endif()    

set( QT_USE_QTNETWORK ON )
set( QT_USE_QTMAIN ON )

if( LOCOPLAY_GUI)
  set( QT_DONT_USE_QTGUI OFF )
  add_definitions( -DLOCOPLAY_GUI -DLOCO_GUI )
else()
  set( QT_DONT_USE_QTGUI ON )
endif()  
if( LOCOPLAY_WKIT )
  set( QT_DONT_USE_QTGUI OFF )
  set( QT_USE_QTWEBKIT ON )
  if( NOT LOCOPLAY_JS_SYNTAX_CHECKER )
    set( QT_USE_QTSCRIPT OFF )  
  endif()  
  set( LOCOPLAY_GUI ON )
  add_definitions( -DLOCO_GUI -DLOCO_WKIT -DLOCOPLAY_GUI -DLOCOPLAY_WKIT ) 
else()
  set( QT_USE_QTSCRIPT ON )
endif()
if( LOCOPLAY_CONSOLE )
  add_definitions( -DLOCOPLAY_CONSOLE ) 
endif()
if( LOCOPLAY_FSYSTEM )
  add_definitions( -DLOCOPLAY_FSYSTEM )
endif()
if( LOCOPLAY_SYSTEM )
  add_definitions( -DLOCOPLAY_SYSTEM ) 
endif()
if( LOCO_NETWORK )
  add_definitions( -DLOCOPLAY_NETWORK ) 
endif()
if( LOCOPLAY_CONTEXT )
  add_definitions( -DLOCOPLAY_CONTEXT ) 
endif()
if( LOCOPLAY_JS_SYNTAX_CHECKER )
  set( QT_USE_QTSCRIPT ON )
  add_definitions( -DLOCOPLAY_JS_SYNTAX_CHECKER -DLOCO_JS_SYNTAX_CHECKER ) 
endif()          

####Qt
find_package(Qt4)
include(${QT_USE_FILE})
               
include_directories( ${QT_LIBRARY_DIR} 
                     ${CMAKE_BINARY_DIR}
                     ${LOCO_INCLUDE_DIR} ) 
link_directories( ${QT_LIBRARY_DIR} ${LOCO_LIB_DIR} )

###RESOURCES
set( RES res/loco.qrc )
QT4_ADD_RESOURCES( RCC_SRCS ${RES} )
SOURCE_GROUP( Resources REGULAR_EXPRESSION "res/.+" ) 

# App
set( SRCS locoplay.cpp ${RCC_SRCS} )

####CONFIGURE EXECUTABLE
add_executable( locoplay ${SRCS} )
target_link_libraries( locoplay ${QT_LIBRARIES} ${LOCOPLAY_LOCO_LIB} ) 
if(WIN32)
  target_link_libraries( locoplay debug qtmaind optimized qtmain )
endif(WIN32)
