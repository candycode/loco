//#SRCHEADER

//WRAPPER FOR A SUBSET OF RtAudio, files copied from STK 4.4.3 
//RtAudio is Copyright (c) 2001-2011 Gary P. Scavone
#include <string>

#include <QObject>
#include <QtPlugin>
#include <QString>
#include <QVariant>
#include <QVariantList>
#include <QVariantMap>

#include "RtAudio.h"

struct IDummy {};
Q_DECLARE_INTERFACE( IDummy, "dummy" )

class RtAudioPlugin : public QObject, public IDummy {
	Q_OBJECT
	Q_INTERFACES( IDummy )
	Q_PROPERTY( int numDevices READ GetNumDevices )
	Q_PROPERTY( int defaultInputDevice READ GetDefaultInputDevice )
	Q_PROPERTY( int defaultOutputDevice READ GetDefaultOutputDevice )
	Q_PROPERTY( bool streamOpen READ IsStreamOpen )
	Q_PROPERTY( bool streamRunning READ IsStreamRunning )
	Q_PROPERTY( int sampleRate READ GetStreamSampleRate )
	Q_PROPERTY( double streamTime READ GetStreamTime )
	Q_PROPERTY( int streamLatency READ GetStreamLatency )
public:
	RtAudioPlugin( QObject* parent = 0 ) : QObject( parent ), adc_( RtAudio::WINDOWS_DS ) {}
	int GetNumDevices() const { return adc_.getDeviceCount(); }
	int GetDefaultInputDevice() const { return adc_.getDefaultInputDevice(); }
	int GetDefaultOutputDevice() const { return adc_.getDefaultOutputDevice(); }
	static int RtAudioInputCBack( void* /*outputBuffer*/, void* inputBuffer, unsigned int nBufferFrames,
                             double streamTime, RtAudioStreamStatus status, void *userData ) {        
        RtAudioPlugin* ap = reinterpret_cast< RtAudioPlugin* >( userData );
        if( status ) {
        	ap->EmitError( "Overflow" );
        	return 0x2;
        }
        ///\todo make it work with any type
        const double* in = reinterpret_cast< const double* >( inputBuffer );
        ///\todo move into class instance, reuse same buffer
        QVariantList out;
        out.reserve( nBufferFrames );
        for( ; nBufferFrames; --nBufferFrames, ++in ) out.push_back( *in );  
        ap->EmitInputReady( out );   
        return 0;   

    }
	static int RtAudioOutputCBack( void* outputBuffer, void* /*inputBuffer*/, unsigned int nBufferFrames,
                             double streamTime, RtAudioStreamStatus status, void *userData ) {        
        RtAudioPlugin* ap = reinterpret_cast< RtAudioPlugin* >( userData );
        if( status ) {
        	ap->EmitError( "Underflow" );
        	return 0x2;
        }
        ///\todo make it work with any type
        double* out = reinterpret_cast< double* >( outputBuffer );
        ///\todo move into class instance, reuse same buffer
        QVariantList in;
		ap->EmitOutputReady( in ); // get output values
        for( int i = 0; i != nBufferFrames && i != in.length(); ++i, ++out ) *out = in[ i ].toDouble();  
     
        return 0;   
    }
    bool IsStreamOpen() const {
    	return adc_.isStreamOpen();
    }
    bool IsStreamRunning() const {
    	return adc_.isStreamRunning();
    }
    double GetStreamTime() const {
    	return adc_.getStreamTime();
    } 
    long GetStreamLatency() const {
    	return adc_.getStreamLatency();
    }
    int GetStreamSampleRate() const {
    	return adc_.getStreamSampleRate();
    } 
signals: 
    void inputReady( const QVariantList& );
	void outputReady( QVariantList& );
    void error( const QString& );              
public slots:
    void openInputStream( const QVariantMap& parameters,
                          const QString& dataType,
                          int sampleRate, 
                          unsigned int sampleFrames ) {
        inputDataType_ = ConvertDataType( dataType );
        RtAudio::StreamParameters params = ConvertStreamParameters( parameters );
        try {
            adc_.openStream( 0, &params, inputDataType_, sampleRate, &sampleFrames,
                            &RtAudioPlugin::RtAudioInputCBack, this );
            adc_.startStream();
        } catch( const RtError& e ) {
        	HandleException( e );
        }                                       	                 	
    }
	void openOutputStream( const QVariantMap& parameters,
                           const QString& dataType,
                           int sampleRate, 
                           unsigned int sampleFrames ) {
        outputDataType_ = ConvertDataType( dataType );
        RtAudio::StreamParameters params = ConvertStreamParameters( parameters );
        try {
            adc_.openStream( &params, 0, outputDataType_, sampleRate, &sampleFrames,
                            &RtAudioPlugin::RtAudioOutputCBack, this );
            adc_.startStream();
        } catch( const RtError& e ) {
        	HandleException( e );
        }                                       	                 	
    }
    void stopStream() {
        try {
            adc_.stopStream();
        }  catch ( const RtError& e ) {
           HandleException( e );
        }
    }
    void abortStream() {
        try {
            adc_.abortStream();
        }  catch ( const RtError& e ) {
           HandleException( e );
        }	
    }
    void closeStream() {
    	 try {
            adc_.closeStream();
        }  catch ( const RtError& e ) {
           HandleException( e );
        }	
    }
  
private:
	void EmitInputReady( const QVariantList& vl ) { emit inputReady( vl ); }
	void EmitOutputReady( QVariantList& vl ) { emit outputReady( vl ); } 
	void EmitError( const QString& msg ) { emit error( msg ); }
    void HandleException( const RtError& e ) {
    	emit error( QString( e.getMessage().c_str() ) );
    }
    RtAudioStreamFlags ConvertDataType( const QString& dt ) {
        ///\todo remove
    	return RTAUDIO_FLOAT64;
    	if( dt == "sint8"  ) { return RTAUDIO_SINT8; }
    	else if( dt == "sint16" ) { return RTAUDIO_SINT16; }
    	else if( dt == "sint24" ) { return RTAUDIO_SINT24; }
    	else if( dt == "sint32" ) { return RTAUDIO_SINT32; }
    	else if( dt == "float32" ) { return RTAUDIO_FLOAT32; }
    	else if( dt == "float64" ) { return RTAUDIO_FLOAT64; }
		EmitError( "Unknown data type: " + dt );
		return RtAudioStreamFlags();
    }
    RtAudio::StreamParameters ConvertStreamParameters( const QVariantMap& pm ) {
        RtAudio::StreamParameters params;
        int deviceId = -1;
        int nChannels = 1;
        int channelOffset = 0;
        if( pm.contains( "deviceId" ) ) deviceId = pm[ "deviceId" ].toInt();
        if ( deviceId < 0 ) deviceId = adc_.getDefaultInputDevice();
        if( pm.contains( "numChannels" ) ) nChannels = pm[ "numChannels" ].toInt();
        if( pm.contains( "channelOffset" ) ) channelOffset = pm[ "channelOffset" ].toInt();
        params.deviceId = deviceId;
        params.nChannels = nChannels;
        params.firstChannel = channelOffset;
        return params;
  
    }
private:
	mutable RtAudio adc_;
    mutable RtAudioStreamFlags inputDataType_;
    mutable RtAudioStreamFlags outputDataType_;
    
};

Q_EXPORT_PLUGIN2( RtAudio, RtAudioPlugin )