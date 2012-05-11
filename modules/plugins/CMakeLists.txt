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

project("loco")

option( LOCOPLUGINS_OSGVIEW "OSG View" OFF )
option( LOCOPLUGINS_BENCHMARK "Benchmark" ON )
option( LOCOPLUGINS_SAMPLE "Sample plugin" OFF)
option( LOCOPLUGINS_RTAUDIO "RtAudio wrapper" OFF)
option( LOCOPLUGINS_SIGSLOT "Signal-Slot test" ON)
option( LOCOPLUGINS_GLVIEW  "OpenGL view" OFF )

if( LOCOPLUGINS_SAMPLE )
  add_subdirectory( sample )
endif()
if( LOCOPLUGINS_OSGVIEW )
  add_subdirectory( osgview )
endif()
if( LOCOPLUGINS_BENCHMARK )
  add_subdirectory( benchmark )
endif()
if( LOCOPLUGINS_RTAUDIO )
  add_subdirectory( rtaudio )
endif()
if( LOCOPLUGINS_SIGSLOT )
  add_subdirectory( sig-slot-test )
endif()
if( LOCOPLUGINS_GLVIEW )
  add_subdirectory( glview )
endif()
