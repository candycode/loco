#include "../include/LocoQGLFormat.h"
namespace loco {
QGLFormat OpenGLFormat( const QVariantMap& fm ) {
    QGLFormat glf;
    if( fm.contains( "accum" ) && fm[ "accum" ].toBool() == true ) {
        glf.setAccum( true );
    } else glf.setAccum( false );
    if( fm.contains( "accumBufferSize" ) ) {
        glf.setAccumBufferSize( fm[ "accumBufferSize" ].toInt() );
    }
    if( fm.contains( "alpha" ) && fm[ "alpha" ].toBool() == true ) {
        glf.setAlpha( true );
    } else glf.setAlpha( false );
    if( fm.contains( "alphaBufferSize" ) ) {
        glf.setAlphaBufferSize( fm[ "alphaBufferSize" ].toInt() );
    }
    if( fm.contains( "redBufferSize" ) ) {
        glf.setRedBufferSize( fm[ "redBufferSize" ].toInt() );
    }
    if( fm.contains( "greenBufferSize" ) ) {
        glf.setGreenBufferSize( fm[ "greenBufferSize" ].toInt() );
    }
    if( fm.contains( "blueBufferSize" ) ) {
        glf.setRedBufferSize( fm[ "blueBufferSize" ].toInt() );
    }
    if( fm.contains( "depth" ) && fm[ "depth" ].toBool() == true ) {
        glf.setDepth( true );
    } else glf.setDepth( false );
    if( fm.contains( "depthBufferSize" ) ) {
        glf.setDepthBufferSize( fm[ "depthBufferSize" ].toInt() );
    }
    if( fm.contains( "directRendering" ) && fm[ "directRendering" ].toBool() == true ) {
        glf.setDirectRendering( true );
    } else glf.setDirectRendering( false );
    if( fm.contains( "doubleBuffer" ) && fm[ "doubleBuffer" ].toBool() == true ) {
        glf.setDoubleBuffer( true );
    } else glf.setDoubleBuffer( false );
    if( fm.contains( "overlay" ) && fm[ "overlay" ].toBool() == true ) {
        glf.setOverlay( true );
    } else glf.setOverlay( false );
    if( fm.contains( "plane" ) ) {
        glf.setPlane( fm[ "plane" ].toInt() );
    }
    if( fm.contains( "profile" ) ) {
        const QString& p =  fm[ "profile" ].toString();
        if( p == "core" ) glf.setProfile( QGLFormat::CoreProfile );
        else if( p == "compatibility" ) glf.setProfile( QGLFormat::CompatibilityProfile );
        else if( p == "no" ) glf.setProfile( QGLFormat::NoProfile );
    }
    if( fm.contains( "rgba" ) && fm[ "rgba" ].toBool() == true ) {
        glf.setRgba( true );
    } else glf.setRgba( false );
    if( fm.contains( "sampleBuffers" ) && fm[ "sampleBuffers" ].toBool() == true ) {
        glf.setSampleBuffers( true );
    } else glf.setSampleBuffers( false );
    if( fm.contains( "samples" ) ) {
        glf.setSamples( fm[ "samples" ].toInt() );
    }
    if( fm.contains( "stencil" ) && fm[ "stencil" ].toBool() == true ) {
        glf.setStencil( true );
    } else glf.setStencil( false );
    if( fm.contains( "stencilBufferSize" ) ) {
        glf.setSamples( fm[ "stenciBufferSize" ].toInt() );
    }
    if( fm.contains( "stereo" ) && fm[ "stereo" ].toBool() == true ) {
        glf.setStereo( true );
    } else glf.setStereo( false );
    if( fm.contains( "swapInterval" ) ) {
        glf.setSwapInterval( fm[ "swapInterval" ].toInt() );
    }
    if( fm.contains( "version" ) ) {
        const QVariantList& v = fm[ "version" ].toList();
        int maj = 0;
        int min = 0;
        if( v.length() > 0 ) maj = v.at( 0 ).toInt();
        if( v.length() > 1 ) min = v.at( 1 ).toInt();
        glf.setVersion( maj, min ); 
    }
    return glf;
}
}
