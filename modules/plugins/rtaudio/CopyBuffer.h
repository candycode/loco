#pragma once

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
	//on V++ -D_SCL_SECURE_NO_WARNINGS to disable warning
	std::copy( i, i + sz, o );
}

void BufferCopy( const QVariantList& inList, stk::StkFrames& outFrames, int sz ) {
	QVariantList::const_iterator in = inList.begin();
	stk::StkFloat* out = &outFrames[ 0 ];
	for( ; sz; ++out, ++in, --sz ) *out = in->toDouble(); 
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

inline void CopyInt8( const QVariantList& in, void* out, int sz ) { 
	char* output = reinterpret_cast< char* >( out );
	for( QVariantList::const_iterator i = in.begin();
		 i != in.end(); ++i, ++output ) {
			 *output = char( i->toInt() );
	}
}
inline void CopyInt16( const QVariantList& in, void* out, int sz ) { 
	short* output = reinterpret_cast< short* >( out );
	for( QVariantList::const_iterator i = in.begin();
		 i != in.end(); ++i, ++output ) {
			 *output = short( i->toInt() );
	}
}
inline void CopyInt32( const QVariantList& in, void* out, int sz ) { 
	int* output = reinterpret_cast< int* >( out );
	for( QVariantList::const_iterator i = in.begin();
		 i != in.end(); ++i, ++output ) {
			 *output = i->toInt();
	}
}
inline void CopyFloat32( const QVariantList& in, void* out, int sz ) { 
	float* output = reinterpret_cast< float* >( out );
	for( QVariantList::const_iterator i = in.begin();
		 i != in.end(); ++i, ++output ) {
			 *output = i->toFloat();
	}
}
inline void CopyFloat64( const QVariantList& in, void* out, int sz ) { 
	double* output = reinterpret_cast< double* >( out );
	for( QVariantList::const_iterator i = in.begin();
		 i != in.end(); ++i, ++output ) {
			 *output = i->toDouble();
	}
}

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

inline void CopyBuffer( const QVariantList& in, stk::StkFrames& outStreams ) {
	BufferCopy( in, outStreams, in.length() );
}