#include "../include/LocoQGLFormat.h"
namespace loco {
QGLFormat OpenGLFormat( const QVariantMap& fm ) {
    QGLFormat glf;
    if( fm.contains( "accum" ) ) {
        glf.setAccum( fm[ "accum" ].toBool() );
    }
    if( fm.contains( "accumBufferSize" ) ) {
        glf.setAccumBufferSize( fm[ "accumBufferSize" ].toInt() );
    }
    if( fm.contains( "alpha" ) ) {
        glf.setAlpha( fm[ "alpha" ].toBool() );
    }
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
    if( fm.contains( "depth" ) ) {
        glf.setDepth( fm[ "depth" ].toBool() );
    }
    if( fm.contains( "depthBufferSize" ) ) {
        glf.setDepthBufferSize( fm[ "depthBufferSize" ].toInt() );
    }
    if( fm.contains( "directRendering" ) ) {
        glf.setDirectRendering( fm[ "directRendering" ].toBool() );
    }
    if( fm.contains( "doubleBuffer" ) ) {
        glf.setDoubleBuffer( fm[ "doubleBuffer" ].toBool() );
    }
    if( fm.contains( "overlay" ) ) {
        glf.setOverlay( fm[ "overlay" ].toBool() );
    }
    if( fm.contains( "plane" ) ) {
        glf.setPlane( fm[ "plane" ].toInt() );
    }
    if( fm.contains( "profile" ) ) {
        const QString& p =  fm[ "profile" ].toString();
        if( p == "core" ) glf.setProfile( QGLFormat::CoreProfile );
        else if( p == "compatibility" ) glf.setProfile( QGLFormat::CompatibilityProfile );
        else if( p == "no" ) glf.setProfile( QGLFormat::NoProfile );
    }
    if( fm.contains( "rgba" ) ) {
        glf.setRgba( fm[ "rgba" ].toBool() );
    }
    if( fm.contains( "sampleBuffers" ) ) {
        glf.setSampleBuffers( fm[ "sampleBuffers" ].toBool() );
    }
    if( fm.contains( "samples" ) ) {
        glf.setSamples( fm[ "samples" ].toInt() );
    }
    if( fm.contains( "stencil" ) ) {
        glf.setStencil( fm[ "stencil" ].toBool() );
    }
    if( fm.contains( "stencilBufferSize" ) ) {
        glf.setSamples( fm[ "stenciBufferSize" ].toInt() );
    }
    if( fm.contains( "stereo" ) ) {
        glf.setStereo( fm[ "stereo" ].toBool() );
    }
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
