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
#include <QStringList>

#include "RtAudio.h"
#include "CopyBuffer.h"


#include <iostream>

struct IDummy {};
Q_DECLARE_INTERFACE( IDummy, "dummy" )

class RtAudioPlugin : public QObject, public IDummy {
	Q_OBJECT
	Q_INTERFACES( IDummy )
	Q_PROPERTY( int numDevices READ GetNumDevices )
	Q_PROPERTY( int defaultInputDevice READ GetDefaultInputDevice )
	Q_PROPERTY( int defaultOutputDevice READ GetDefaultOutputDevice )
	Q_PROPERTY( bool streamOpen READ IsStreamOpen )
	Q_PROPERTY( unsigned int bufferSize READ GetBufferSize )
	Q_PROPERTY( bool streamRunning READ IsStreamRunning )
	Q_PROPERTY( int sampleRate READ GetStreamSampleRate )
	Q_PROPERTY( double streamTime READ GetStreamTime )
	Q_PROPERTY( int streamLatency READ GetStreamLatency )
	Q_PROPERTY( QVariantList input READ GetInput )
	Q_PROPERTY( QVariantList output READ GetOutput WRITE SetOutput )
	Q_PROPERTY( int status READ GetStatus )
	Q_PROPERTY( QVariantMap devices READ GetDevices )
public:
	RtAudioPlugin( QObject* parent = 0 ) : QObject( parent ), 
		adc_( RtAudio::WINDOWS_DS ), bufferSize_( 0 ), status_( 0 ) {}
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
        QVariantList out;
        out.reserve( nBufferFrames );
        CopyBuffer( inputBuffer, out, nBufferFrames, ap->GetInputType() );
        ap->EmitInputReady( out );   
        return 0;   

    }
	static int RtAudioInputOutputCBack( void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
                                        double streamTime, RtAudioStreamStatus status, void *userData ) { 
       
        RtAudioPlugin* ap = reinterpret_cast< RtAudioPlugin* >( userData );
		if( status == RTAUDIO_INPUT_OVERFLOW ) {
        	ap->status_ = RTAUDIO_INPUT_OVERFLOW;
        } else if( status == RTAUDIO_OUTPUT_UNDERFLOW ) {
        	ap->status_ = RTAUDIO_OUTPUT_UNDERFLOW;
        } else ap->status_ = 0;
		QVariantList& input = ap->input_;
		CopyBuffer( inputBuffer, input, nBufferFrames, ap->GetInputType() );
		ap->EmitFilter();
        const QVariantList& output = ap->GetOutput();
        CopyBuffer( output, outputBuffer, nBufferFrames, ap->GetOutputType() );
        return 0;   
    }
	static int RtAudioOutputCBack( void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
                             double streamTime, RtAudioStreamStatus status, void *userData ) {        
        RtAudioPlugin* ap = reinterpret_cast< RtAudioPlugin* >( userData );
        if( status ) {
        	ap->EmitError( "Underflow" );
        	return 0x2;
        }
        ap->EmitOutputReady(); // get output values
        const QVariantList& output = ap->GetOutput();
        CopyBuffer( output, outputBuffer, nBufferFrames, ap->GetOutputType() );
        return 0;   
    }
	int GetStatus() const { return int( status_ ); } 
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
	const QVariantList& GetOutput() const { return output_; }
	const QVariantList& GetInput() const { return input_; }
	void SetOutput( const QVariantList& out ) { output_ = out; }
	unsigned GetBufferSize() const { return bufferSize_; }
	QVariantList GetDevices() const {
	    QVariantList dl;
		for( int i = 0; i != GetNumDevices(); ++i ) {
			dl.push_back( getDeviceInfo( i ) );
		}
		return dl;
	}
signals: 
    void inputReady( const QVariantList& );
	void outputReady();
	void filter();
    void error( const QString& ) const;              
public slots:
	QVariantMap getDeviceInfo( unsigned id ) const {
		if( id > unsigned( GetNumDevices() - 1 ) ) {
		    emit error( "Device does not exist" );
			return QVariantMap();
		}
		RtAudio::DeviceInfo di = adc_.getDeviceInfo( id );
	    QVariantMap dim;
		dim[ "probed" ] = di.probed;
		dim[ "name"   ] = QString( di.name.c_str() );
		dim[ "outputChannels" ] = di.outputChannels;
		dim[ "inputChannels"  ] = di.inputChannels;
		dim[ "duplexChannels" ] = di.duplexChannels;
		dim[ "defaultOutput"  ] = di.isDefaultOutput;
		dim[ "defaultInput"   ] = di.isDefaultInput;
		QVariantList sr;
		for( std::vector< unsigned >::const_iterator i = di.sampleRates.begin();
			 i != di.sampleRates.end(); ++i ) {
		    sr.push_back( *i ); 
		}
		dim[ "sampleRates" ] = sr;
		QStringList af;
		if( di.nativeFormats | RTAUDIO_SINT8   ) af.push_back( "sint8"   );
		if( di.nativeFormats | RTAUDIO_SINT16  ) af.push_back( "sint16"  );
		if( di.nativeFormats | RTAUDIO_SINT24  ) af.push_back( "sint24"  );
		if( di.nativeFormats | RTAUDIO_SINT32  ) af.push_back( "sint32"  );
		if( di.nativeFormats | RTAUDIO_FLOAT32 ) af.push_back( "float32" );
		if( di.nativeFormats | RTAUDIO_FLOAT64 ) af.push_back( "float64" );
        dim[ "nativeFormats" ] = af;
	}
	void startStream() { adc_.startStream(); }
    void openInputStream( const QVariantMap& parameters,
                          const QString& dataType,
                          int sampleRate, 
                          unsigned int sampleFrames ) {
        inputDataType_ = ConvertDataType( dataType );
        RtAudio::StreamParameters params = ConvertStreamParameters( parameters );
        try {
            adc_.openStream( &params, &params, inputDataType_, sampleRate, &sampleFrames,
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
	void openIOStream( const QString& dataType,
                       int sampleRate, 
                       unsigned int sampleFrames,
					   const QVariantMap& inParameters = QVariantMap(),
		               const QVariantMap& outParameters = QVariantMap() ) {
        inputDataType_  = ConvertDataType( dataType );
	    outputDataType_ = ConvertDataType( dataType );
        RtAudio::StreamParameters inParams = ConvertStreamParameters( inParameters );
		RtAudio::StreamParameters outParams = ConvertStreamParameters( outParameters );
		RtAudio::StreamOptions so;
		so.flags = RTAUDIO_MINIMIZE_LATENCY | RTAUDIO_HOG_DEVICE | RTAUDIO_SCHEDULE_REALTIME;
	    try {
            adc_.openStream( &outParams, &inParams, outputDataType_, sampleRate, &sampleFrames,
                            &RtAudioPlugin::RtAudioInputOutputCBack, this, &so );
			input_.reserve( sampleFrames );
			output_.reserve( sampleFrames );
			for( int i = 0; i != sampleFrames; ++i ) {
				input_.push_back( QVariant() );
				output_.push_back( QVariant() );
			}
			bufferSize_ = sampleFrames;
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
	void EmitFilter() { emit filter(); }
	void EmitInputReady( const QVariantList& vl ) { emit inputReady( vl ); }
	void EmitOutputReady() { emit outputReady(); } 
	void EmitError( const QString& msg ) { emit error( msg ); }
    void HandleException( const RtError& e ) {
    	emit error( QString( e.getMessage().c_str() ) );
    }
	RtAudioStreamFlags GetInputType() const { return inputDataType_; }
    RtAudioStreamFlags GetOutputType() const { return outputDataType_; }
    RtAudioStreamFlags ConvertDataType( const QString& dt ) {
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
        if ( deviceId < 0 ) deviceId = 0;
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
	QVariantList output_;   
	QVariantList input_;
	unsigned bufferSize_;
	RtAudioStreamStatus status_;
};

Q_EXPORT_PLUGIN2( RtAudio, RtAudioPlugin )