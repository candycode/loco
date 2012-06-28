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

project("libloco")

set( LINC include )
set( LSRC src )

if( CMAKE_COMPILER_IS_GNUCXX AND NOT APPLE )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" )
endif()

####  LIB CONFIGURATION
option( LOCO_WKIT "Build with QtWebKit instead of QtScript, requires QtGUI and X(or xvfb) on Linux" ON )
option( LOCO_GUI "GUI support, required by WKIT" ON )
option( LOCO_CONSOLE "stdin/out/err" ON )
option( LOCO_FSYSTEM "Access to filesystem" ON )
option( LOCO_SYSTEM "System access to execute programs, required to handle system events." ON )
option( LOCO_CONTEXT "Adds context to javscript" ON )
option( LOCO_NETWORK "Access to network" ON  )
option( LOCO_DYNAMIC "Dynamic library" OFF )
option( LOCO_SCRIPT "Enable QScript; not needed if WebKit enabled" OFF )
option( LOCO_JS_SYNTAX_CHECKER "Add SyntaxChecker module - enables QScript" ON )
option( LOCO_SSL "SSL support" OFF )

if( LOCO_WKIT )
  set( LOCO_LIB_NAME "loco-wkit" CACHE STRING "LoCO library name" )
else()
  set( LOCO_LIB_NAME "loco" CACHE STRING "LoCO library name" )
endif()    

set( QT_USE_QTNETWORK ON )

if( LOCO_GUI )
  set( QT_USE_QTOPENGL ON )
  set( QT_DONT_USE_QTGUI OFF )
  add_definitions( -DLOCO_GUI ) 
else()
  set( QT_DONT_USE_QTGUI ON )
endif()  
if( LOCO_WKIT )
  set( QT_DONT_USE_QTGUI OFF )
  set( QT_USE_QTWEBKIT ON )
  set( LOCO_GUI ON )
  add_definitions( -DLOCO_GUI -DLOCO_WKIT ) 
else()
  set( LOCO_SCRIPT ON )
endif()
if( LOCO_SCRIPT )
  set( QT_USE_QTSCRIPT ON )
  add_definitions( -DLOCO_SCRIPT ) 
endif()
if( LOCO_CONSOLE )
  add_definitions( -DLOCO_CONSOLE ) 
endif()
if( LOCO_FSYSTEM )
  add_definitions( -DLOCO_FSYSTEM ) 
endif()
if( LOCO_SYSTEM )
  add_definitions( -DLOCO_SYSTEM ) 
endif()
if( LOCO_NETWORK )
  add_definitions( -DLOCO_NETWORK ) 
endif()
if( LOCO_CONTEXT )
  add_definitions( -DLOCO_CONTEXT ) 
endif()         
if( LOCO_JS_SYNTAX_CHECKER )
  set( QT_USE_QTSCRIPT ON )
endif()  
if( LOCO_SSL )
  add_definitions( -DLOCO_SSL ) 
endif()
####Qt
find_package(Qt4)
include(${QT_USE_FILE})
               
include_directories( ${QT_LIBRARY_DIR} 
                     include ) 
link_directories( ${QT_LIBRARY_DIR} )

####SETUP LIB
if( LOCO_CONSOLE )
  set( LOCO_CONSOLE_MOC_HEADERS ${LINC}/LocoConsole.h )
endif()
if( LOCO_FSYSTEM )
  set( LOCO_FSYSTEM_MOC_HEADERS ${LINC}/LocoFileSystem.h 
       ${LINC}/LocoTempFile.h ${LINC}/LocoFile.h ${LINC}/LocoDir.h ${LINC}/LocoFSWatcher.h )
  set( LOCO_FSYSTEM_SRCS ${LSRC}/LocoFileSystem.cpp ${LSRC}/LocoFile.cpp )
  set( LOCO_FSYSTEM_INCLUDE ${LINC}/LocoFileAccessManager.h )
endif()
if( LOCO_SYSTEM )
  set( LOCO_SYSTEM_MOC_HEADERS ${LINC}/LocoSystem.h ${LINC}/LocoProcess.h ${LINC}/LocoEventLoop.h )
  set( LOCO_SYSTEM_SRCS ${LSRC}/LocoSystem.cpp ${LSRC}/LocoProcess.cpp )
endif()
if( LOCO_WKIT )
  set( LOCO_WKIT_MOC_HEADERS ${LINC}/LocoWebWindow.h 
       ${LINC}/LocoWebKitJSCore.h ${LINC}/LocoWebKitJSCoreWrapper.h ${LINC}/LocoWebView.h
       ${LINC}/LocoDynamicWebPluginFactory.h ${LINC}/LocoWebElement.h ${LINC}/LocoWebPage.h
       ${LINC}/LocoGraphicsWebWindow.h )
  set( LOCO_WKIT_HEADERS ${LINC}/LocoWebKitAttributeMap.h ${LINC}/LocoStaticWebPluginFactory.h )
  set( LOCO_WKIT_SRCS ${LSRC}/LocoWebKitAttributeMap.cpp ${LSRC}/LocoWebView.cpp ${LSRC}/LocoGraphicsWebWindow.cpp )
endif()
if( LOCO_GUI )
  set( LOCO_GUI_MOC_HEADERS ${LINC}/LocoGUI.h ${LINC}/LocoMainWindow.h ${LINC}/LocoWidgetWrapper.h ${LINC}/LocoWrappedWidget.h
       ${LINC}/LocoLayout.h ${LINC}/LocoWidget.h ${LINC}/LocoGraphicsScene.h ${LINC}/LocoGraphicsView.h
       ${LINC}/LocoWrappedGraphicsWidget.h ${LINC}/LocoGraphics.h ${LINC}/LocoImage2D.h )
  set( LOCO_GUI_SRCS ${LSRC}/LocoGUI.cpp ${LSRC}/LocoWrappedWidget.cpp ${LSRC}/LocoLayout.cpp ${LSRC}/LocoWindowAttributes.cpp
       ${LSRC}/LocoWindowFlags.cpp ${LSRC}/LocoGraphics.cpp )
endif()
if( LOCO_SCRIPT )
  set( LOCO_SCRIPT_MOC_HEADERS ${LINC}/LocoQScriptInterpreter.h )
endif()
if( LOCO_JS_SYNTAX_CHECKER )
  set( LOCO_JS_SYNTAX_CHECKER_MOC_HEADERS ${LINC}/LocoJSSyntaxChecker.h )
endif()  

set( MOC_HEADERS
  ${LOCO_CONSOLE_MOC_HEADERS}
  ${LINC}/LocoContext.h ${LINC}/LocoJSContext.h ${LINC}/EWL.h ${LINC}/LocoObject.h ${LINC}/LocoFilter.h ${LINC}/LocoScriptFilter.h
  ${LOCO_FSYSTEM_MOC_HEADERS} ${LOCO_SYSTEM_MOC_HEADERS} 
  ${LINC}/LocoNetworkAccessManager.h ${LINC}/LocoApp.h ${LINC}/LocoObjectInfo.h ${LINC}/LocoIJSInterpreter.h
  ${LOCO_WKIT_MOC_HEADERS} ${LOCO_GUI_MOC_HEADERS}
  ${LOCO_SCRIPT_MOC_HEADERS} ${LOCO_JS_SYNTAX_CHECKER_MOC_HEADERS}
  ${LINC}/LocoScriptNetworkRequestHandler.h ${LINC}/LocoContextThread.h 
  ${LINC}/LocoDataType.h ${LINC}/LocoContextThreadLoop.h
  ${LINC}/LocoNetwork.h ${LINC}/LocoTimer.h ${LINC}/Arrays/LocoFloat64Array.h
  ${LINC}/Arrays/LocoFloat32Array.h ${LINC}/Arrays/LocoIntArray.h
  ${LINC}/Arrays/LocoUIntArray.h ${LINC}/Arrays/LocoShortArray.h
  ${LINC}/Arrays/LocoUShortArray.h ${LINC}/Arrays/LocoByteArray.h
  ${LINC}/LocoGraphicsView.h ) 

QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} ) 

# Lib 
set( SRCS ${LSRC}/LocoObject.cpp  ${LOCO_FSYSTEM_SRCS} ${LOCO_WKIT_SRCS} ${LSRC}/LocoApp.cpp
     ${LOCO_SYSTEM_SRCS} ${LOCO_GUI_SRCS} ${LSRC}/LocoContext.cpp ${LSRC}/LocoNetworkAccessManager.cpp
     ${LSRC}/LocoScriptFilter.cpp ${LSRC}/LocoNetwork.cpp ${LSRC}/LocoURLUtility.cpp
     ${LSRC}/LocoQGLFormat.cpp )    
    
set( SRCS_INCLUDE ${MOC_HEADERS} ${LOCO_WKIT_HEADERS} ${LOCO_FSYSTEM_INCLUDE} 
     ${LINC}/LocoDefaultJSInit.h ${LINC}/LocoApp.h ${LINC}/LocoINetworkRequestHandler.h
     ${LINC}/LocoURLUtility.h ${LINC}/Arrays/LocoArray.h ${LINC}/LocoQGLFormat.h )

####CONFIGURE LIBRARY
if(LOCO_DYNAMIC)
  add_library( ${LOCO_LIB_NAME} SHARED ${SRCS} ${SRCS_INCLUDE} ${MOC_SRCS} ${RCC_SRCS} )
else()
  add_library( ${LOCO_LIB_NAME} ${SRCS} ${SRCS_INCLUDE} ${MOC_SRCS} ${RCC_SRCS} )
endif()
target_link_libraries( ${LOCO_LIB_NAME} ${QT_LIBRARIES} ) 
