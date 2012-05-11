#pragma once
////////////////////////////////////////////////////////////////////////////////
//Copyright (c) 2012, Ugo Varetto
//All rights reserved.
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions are met:
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder nor the
//      names of its contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//DISCLAIMED. IN NO EVENT SHALL UGO VARETTO BE LIABLE FOR ANY
//DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>

#include <QVariant>
#include <QVariantList>

#include "stk/RtAudio.h"
#include "stk/Stk.h"

template < typename T > void BufferCopy( const void* in, QVariantList& out, int sz ) {
    const T* i = reinterpret_cast< const T* >( in );
	for( int c = 0; c != sz; ++c, ++i ) out[ c ] = *i; 
}

template < typename T > void BufferCopy( const void* in, void* out, int sz ) {
    const T* i = reinterpret_cast< const T* >( in );
	T* o = reinterpret_cast< T* >( out );
	//on VC++ -D_SCL_SECURE_NO_WARNINGS to disable warning
	std::copy( i, i + sz, o );
}

void BufferCopy( const QVariantList& inList, stk::StkFrames& outFrames,
		         int sz, stk::StkFloat scaling ) {
	QVariantList::const_iterator in = inList.begin();
	stk::StkFloat* out = &outFrames[ 0 ];
	for( ; sz; ++out, ++in, --sz ) {
		*out = scaling * stk::StkFloat( in->toDouble() );
	}
}

template < typename T >
void BufferCopy( const QVariantList& inList, void* out, int sz ) {
	QVariantList::const_iterator in = inList.begin();
	T* o = reinterpret_cast< T* >( out );
	for( ; sz; ++o, ++in, --sz ) *o = T( in->toDouble() );
}

inline void CopyInt8( const void* in, QVariantList& out, int sz ) { BufferCopy< char >( in, out, sz ); }
inline void CopyInt16( const void* in, QVariantList& out, int sz ) { BufferCopy< short >( in, out, sz ); }
inline void CopyInt24( const void* in, QVariantList& out, int sz ) { BufferCopy< int >( in, out, sz ); }
inline void CopyInt32( const void* in, QVariantList& out, int sz ) { BufferCopy< int >( in, out, sz ); }
inline void CopyFloat32( const void* in, QVariantList& out, int sz ) { BufferCopy< float >( in, out, sz ); }
inline void CopyFloat64( const void* in, QVariantList& out, int sz ) { BufferCopy< double >( in, out, sz ); }

inline void CopyInt8( const void* in, void* out, int sz ) { BufferCopy< char >( in, out, sz ); }
inline void CopyInt16( const void* in, void* out, int sz ) { BufferCopy< short >( in, out, sz ); }
inline void CopyInt24( const void* in, void* out, int sz ) { BufferCopy< int >( in, out, sz ); }
inline void CopyInt32( const void* in, void* out, int sz ) { BufferCopy< int >( in, out, sz ); }
inline void CopyFloat32( const void* in, void* out, int sz ) { BufferCopy< float >( in, out, sz ); }
inline void CopyFloat64( const void* in, void* out, int sz ) { BufferCopy< double >( in, out, sz ); }

/*inline void CopyInt8( const QVariantList& in, void* out, int sz ) {
	char* output = reinterpret_cast< char* >( out );
	for( QVariantList::const_iterator i = in.begin();
		 i != in.end(); ++i, ++output ) {
			 *output = char( i->toDouble() );
	}
}
inline void CopyInt16( const QVariantList& in, void* out, int sz ) { 
	short* output = reinterpret_cast< short* >( out );
	for( QVariantList::const_iterator i = in.begin();
		 i != in.end(); ++i, ++output ) {
			 *output = short( i->toDouble() );
	}
}
inline void CopyInt32( const QVariantList& in, void* out, int sz ) {
	int* output = reinterpret_cast< int* >( out );
	for( QVariantList::const_iterator i = in.begin();
		 i != in.end(); ++i, ++output ) {
			 *output = int( i->toDouble() );
	}
}
inline void CopyFloat32( const QVariantList& in, void* out, int sz ) { 
	float* output = reinterpret_cast< float* >( out );
	for( QVariantList::const_iterator i = in.begin();
		 i != in.end(); ++i, ++output ) {
			 *output = float( i->toDouble() );
	}
}
inline void CopyFloat64( const QVariantList& in, void* out, int sz ) { 
	double* output = reinterpret_cast< double* >( out );
	for( QVariantList::const_iterator i = in.begin();
		 i != in.end(); ++i, ++output ) {

	    // std::cout <<  i->toDouble() << ' ';
			 *output = i->toDouble();
	}
}*/

inline void CopyBuffer( const void* in, QVariantList& out, int sz, RtAudioStreamFlags type ) {
	switch( type ) {
	case RTAUDIO_SINT8: CopyInt8( in, out, sz );
		                break;
	case RTAUDIO_SINT16: CopyInt16( in, out, sz );
		                 break;
	case RTAUDIO_SINT24: CopyInt32( in, out, sz );
		                 break;
    case RTAUDIO_SINT32: CopyInt32( in, out, sz );
		                 break;
    case RTAUDIO_FLOAT32: CopyFloat32( in, out, sz );
		                  break;
    case RTAUDIO_FLOAT64: CopyFloat64( in, out, sz );
		                  break;
	default: break;
	}
}

inline void CopyBuffer( const QVariantList& in, void* out,  int sz, RtAudioStreamFlags type ) {
	switch( type ) {
	case RTAUDIO_SINT8: BufferCopy< char >( in, out, sz);//CopyInt8( in, out, sz );
		                break;
	case RTAUDIO_SINT16: BufferCopy< short >( in, out, sz);//CopyInt16( in, out, sz );
		                 break;
	case RTAUDIO_SINT24: BufferCopy< int >( in, out, sz);//CopyInt32( in, out, sz );
		                 break;
    case RTAUDIO_SINT32: BufferCopy< int >( in, out, sz);//CopyInt32( in, out, sz );
		                 break;
    case RTAUDIO_FLOAT32: BufferCopy< float >( in, out, sz);//CopyFloat32( in, out, sz );
		                  break;
    case RTAUDIO_FLOAT64: BufferCopy< double >( in, out, sz);//CopyFloat64( in, out, sz );
		                  break;
	default: break;
	}
}

inline void CopyBuffer( const void* in, void* out, int sz, RtAudioStreamFlags type ) {
	switch( type ) {
	case RTAUDIO_SINT8: CopyInt8( in, out, sz );
		                break;
	case RTAUDIO_SINT16: CopyInt16( in, out, sz );
		                 break;
	case RTAUDIO_SINT24: CopyInt32( in, out, sz );
		                 break;
    case RTAUDIO_SINT32: CopyInt32( in, out, sz );
		                 break;
    case RTAUDIO_FLOAT32: CopyFloat32( in, out, sz );
		                  break;
    case RTAUDIO_FLOAT64: CopyFloat64( in, out, sz );
		                  break;
	default: break;
	}
}

inline void CopyBuffer( const stk::StkFrames& inStreams, QVariantList& out ) {
	BufferCopy< stk::StkFloat >( &inStreams[ 0 ], out, inStreams.channels() * inStreams.size() );
}

inline void CopyBuffer( const QVariantList& in, stk::StkFrames& outStreams, stk::StkFloat scaling ) {
	BufferCopy( in, outStreams, in.length(), scaling );
}
