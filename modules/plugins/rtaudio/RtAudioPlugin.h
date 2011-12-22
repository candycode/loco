//#SRCHEADER

//Wrapper for RtAudio, files copied from STK 4.4.3 
//RtAudio is Copyright (c) 2001-2011 Gary P. Scavone, see included readme
#include <string>

#include <QObject>
#include <QtPlugin>
#include <QString>
#include <QVariant>
#include <QVariantList>
#include <QVariantMap>
#include <QStringList>

#include "stk/RtAudio.h"
#include "stk/FileRead.h"
#include "stk/FileWrite.h"
#include "stk/Stk.h"
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
	Q_PROPERTY( QVariantList devices READ GetDevices )
	Q_PROPERTY( unsigned sampleRate READ GetSampleRate WRITE SetSampleRate )
public:
	RtAudioPlugin( QObject* parent = 0 ) : QObject( parent ), 
#if defined( Q_OS_WIN )
	    adc_( RtAudio::WINDOWS_DS ),
#elif defined( Q_OS_MAC )
	    adc_( RtAudio::MACOSX_CORE ),
#elif defined( Q_OS_LINUX )
	    adc_( RtAudio::LINUX_ALSA ),
#endif
		bufferSize_( 0 ), status_( 0 ),
	    inputChannels_( 0 ), outputChannels_( 0 ), userData_( 0 ) {}
	int GetNumDevices() const { return adc_.getDeviceCount(); }
	int GetDefaultInputDevice() const { return adc_.getDefaultInputDevice(); }
	int GetDefaultOutputDevice() const { return adc_.getDefaultOutputDevice(); }
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
	void SetSampleRate( unsigned int sr ) const {
	  	stk::Stk::setSampleRate( sr );
	}
	unsigned GetSampleRate() const {
	   	return stk::Stk::sampleRate();
	}
signals: 
    void inputReady();
	void outputReady();
	void filter();
	void streamData( void* outputBuffer, void* inputBuffer,
		             unsigned nBufferFrames, double streamTime,
					 unsigned status, void *userData, int* returnValue ); 
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
		return dim;
	}
	void startStream() { adc_.startStream(); }
    void openInputStream( const QString& dataType,
                          int sampleRate, 
                          unsigned int sampleFrames,
						  const QVariantMap& parameters = QVariantMap(),
					      const QVariantMap& options = QVariantMap() ) {
        inputDataType_  = ConvertDataType( dataType );
        RtAudio::StreamParameters inParams = ConvertStreamParameters( parameters );
		RtAudio::StreamOptions so = ConvertStreamOptions( options );
	    try {
            adc_.openStream( 0, &inParams, inputDataType_, sampleRate, &sampleFrames,
                            &RtAudioPlugin::RtAudioInputCBack, this, &so );
			input_.clear();
			input_.reserve( sampleFrames * inParams.nChannels );
			for( int i = 0; i != sampleFrames * inParams.nChannels; ++i ) {
				input_.push_back( QVariant() );
			}
			bufferSize_ = sampleFrames;
			inputChannels_ = inParams.nChannels;
        } catch( const RtError& e ) {
        	HandleException( e );
        }                                                                    	                 	
    }
	void openOutputStream( const QString& dataType,
                           int sampleRate, 
                           unsigned int sampleFrames,
						   const QVariantMap& parameters = QVariantMap(),
					       const QVariantMap& options = QVariantMap() ) {
        outputDataType_  = ConvertDataType( dataType );
		RtAudio::StreamParameters outParams = ConvertStreamParameters( parameters );
		RtAudio::StreamOptions so = ConvertStreamOptions( options );
		try {
			adc_.openStream( &outParams, 0, outputDataType_, sampleRate, &sampleFrames,
							&RtAudioPlugin::RtAudioOutputCBack, this, &so );
			output_.clear();
			output_.reserve( sampleFrames * outParams.nChannels );
			for( int i = 0; i != sampleFrames * outParams.nChannels; ++i ) {
				output_.push_back( QVariant() );
			}
			bufferSize_ = sampleFrames;
			outputChannels_ = outParams.nChannels;
		} catch( const RtError& e ) {
			HandleException( e );
		}                                                          	                 	
    }
	void openIOStream( const QString& dataType,
                       int sampleRate, 
                       unsigned int sampleFrames,
					   const QVariantMap& inParameters = QVariantMap(),
		               const QVariantMap& outParameters = QVariantMap(),
					   const QVariantMap& options = QVariantMap() ) {
        inputDataType_  = ConvertDataType( dataType );
	    outputDataType_ = ConvertDataType( dataType );
        RtAudio::StreamParameters inParams = ConvertStreamParameters( inParameters );
		RtAudio::StreamParameters outParams = ConvertStreamParameters( outParameters );
	    RtAudio::StreamOptions so = ConvertStreamOptions( options );
	    try {
            adc_.openStream( &outParams, &inParams, outputDataType_, sampleRate, &sampleFrames,
                            &RtAudioPlugin::RtAudioInputOutputCBack, this, &so );
			input_.clear();
			output_.clear();
			input_.reserve( sampleFrames * inParams.nChannels );
			output_.reserve( sampleFrames * outParams.nChannels );
			for( int i = 0; i != sampleFrames * inParams.nChannels; ++i ) {
				input_.push_back( QVariant() );
			}
			for( int i = 0; i != sampleFrames * outParams.nChannels; ++i ) {
				output_.push_back( QVariant() );
			}				
			bufferSize_ = sampleFrames;
			inputChannels_ = inParams.nChannels;
			outputChannels_ = inParams.nChannels;
        } catch( const RtError& e ) {
        	HandleException( e );
        }                                       	                 	
    }
	void openStream( const QString& dataType,
                     int sampleRate, 
                     unsigned int sampleFrames,
					 bool input,
					 bool output,
					 QObject* userData,
					 const QVariantMap& inParameters = QVariantMap(),
		             const QVariantMap& outParameters = QVariantMap(),
					 const QVariantMap& options = QVariantMap() ) {
        inputDataType_  = ConvertDataType( dataType );
	    outputDataType_ = ConvertDataType( dataType );
        RtAudio::StreamParameters inParams = ConvertStreamParameters( inParameters );
		RtAudio::StreamParameters outParams = ConvertStreamParameters( outParameters );
	    RtAudio::StreamOptions so = ConvertStreamOptions( options );
	    try {
			RtAudio::StreamParameters* in  = input ? &inParams : 0;
			RtAudio::StreamParameters* out = output ? &outParams : 0;
            adc_.openStream( in, out, outputDataType_, sampleRate, &sampleFrames,
                             &RtAudioPlugin::RtAudioCBack, userData, &so );
		    bufferSize_ = sampleFrames;
			inputChannels_ = inParams.nChannels;
			outputChannels_ = inParams.nChannels;
        } catch( const RtError& e ) {
        	HandleException( e );
        }                                       	                 	
    }
	QVariantMap readFile( const QString& fileName, bool normalize = false ) const {
		QVariantMap audioData;
		try {
			QVariantList data;		
			stk::FileRead fr( fileName.toStdString() );
			audioData[ "numChannels" ] = fr.channels();
			audioData[ "frames" ] = quint64( fr.fileSize() );
			audioData[ "type" ] = ConvertFormat( fr.format() );
			audioData[ "rate" ] = fr.fileRate();
			audioData[ "normalized" ] = normalize;
			stk::StkFrames frames( fr.fileSize(), fr.channels() );
			fr.read( frames, 0, normalize );
			data.reserve( frames.size() * frames.channels() );
			for( int i = 0; i != frames.size() * frames.channels(); ++i ) {
				data.push_back( QVariant() );
			}
			CopyBuffer( frames, data );
			audioData[ "data" ] = data;
		
		} catch( const stk::StkError& err ) {
			error( err.getMessageCString() );
		}
		return audioData;
	}
	void writeFile( const QString& fileName, const QVariantMap& info ) const {
		try {
			QVariantList data = info[ "data" ].toList();
			const unsigned nChannels = info[ "numChannels" ].toUInt();
			const QString format = info[ "type" ].toString();
			stk::StkFloat rate = stk::StkFloat( info[ "rate" ].toDouble() );
			stk::FileWrite fw( fileName.toStdString(),
					           nChannels, stk::FileWrite::FILE_WAV,
					           ConvertFormat( format ) );
			stk::StkFrames frames( data.length() / nChannels, nChannels );
			frames.setDataRate( rate );
			stk::StkFloat scaling = ComputeScaling( format );
			CopyBuffer( data, frames, scaling );
			fw.write( frames );
		} catch( const stk::StkError& err ) {
			error( err.getMessageCString() );
		}
	}
	bool isStreamOpen() const {
	    return adc_.isStreamOpen();
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
	static int RtAudioInputCBack( void* /*outputBuffer*/, void* inputBuffer, unsigned int nBufferFrames,
                                  double streamTime, RtAudioStreamStatus status, void *userData ) {        
        RtAudioPlugin* ap = reinterpret_cast< RtAudioPlugin* >( userData );
		ap->status_ = status;
		QVariantList& input = ap->input_;
		CopyBuffer( inputBuffer, input, nBufferFrames * ap->inputChannels_, ap->inputDataType_ );
		ap->EmitInputReady();
        return 0;   

    }
	static int RtAudioInputOutputCBack( void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
                                        double streamTime, RtAudioStreamStatus status, void *userData ) { 
       std::cout << nBufferFrames << std::endl; 
        RtAudioPlugin* ap = reinterpret_cast< RtAudioPlugin* >( userData );
		ap->status_ = status;
		QVariantList& input = ap->input_;
		CopyBuffer( inputBuffer, input, nBufferFrames * ap->inputChannels_, ap->inputDataType_ );
		ap->EmitFilter();
        const QVariantList& output = ap->output_;
        CopyBuffer( output, outputBuffer, nBufferFrames * ap->outputChannels_, ap->outputDataType_ );
        return 0;
    }
	static int RtAudioOutputCBack( void* outputBuffer, void* /*inputBuffer*/, unsigned int /*nBufferFrames*/,
                             double streamTime, RtAudioStreamStatus status, void *userData ) {   
        RtAudioPlugin* ap = reinterpret_cast< RtAudioPlugin* >( userData );
		ap->status_ = status;
		ap->EmitOutputReady();
        const QVariantList& output = ap->output_;
		CopyBuffer( output, outputBuffer, output.length(), ap->outputDataType_ );
        return 0;
    }
	static int RtAudioCBack( void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
                             double streamTime, RtAudioStreamStatus status, void *userData ) {        
        
        int ret = 0;
		RtAudioPlugin* ap = reinterpret_cast< RtAudioPlugin* >( userData );
		ap->status_ = status;
		ap->EmitStreamData( outputBuffer, inputBuffer, nBufferFrames, streamTime, status, 
			                ap->userData_, &ret );
        return 0;   
    }
	void EmitFilter() { emit filter(); }
	void EmitInputReady() { emit inputReady(); }
	void EmitOutputReady() { emit outputReady(); }
	void EmitStreamData( void* outputBuffer, void* inputBuffer,
		                 unsigned nBufferFrames, double streamTime,
						 unsigned status, void *userData, int* returnValue ) {
	    emit streamData( outputBuffer, inputBuffer, nBufferFrames, streamTime,
					     status, userData, returnValue ); 
	}
	void EmitError( const QString& msg ) const { emit error( msg ); }
    void HandleException( const RtError& e ) {
    	emit error( QString( e.getMessage().c_str() ) );
    }
	RtAudioStreamFlags GetInputType() const { return inputDataType_; }
    RtAudioStreamFlags GetOutputType() const { return outputDataType_; }
    RtAudioStreamFlags ConvertDataType( const QString& dt ) const {
      	if( dt == "sint8"  ) { return RTAUDIO_SINT8; }
    	else if( dt == "sint16" ) { return RTAUDIO_SINT16; }
    	else if( dt == "sint24" ) { return RTAUDIO_SINT24; }
    	else if( dt == "sint32" ) { return RTAUDIO_SINT32; }
    	else if( dt == "float32" ) { return RTAUDIO_FLOAT32; }
    	else if( dt == "float64" ) { return RTAUDIO_FLOAT64; }
		else {
			EmitError( "Unknown data type: " + dt );
		    return RtAudioStreamFlags();
		}
    }
	QString ConvertFormat( stk::Stk::StkFormat dt ) const {
	    if( dt == stk::Stk::STK_SINT8  ) { return "sint8"; }
    	else if( dt == stk::Stk::STK_SINT16 ) { return "sint16"; }
    	else if( dt == stk::Stk::STK_SINT24 ) { return "sint24"; }
    	else if( dt == stk::Stk::STK_SINT32 ) { return "sint32"; }
    	else if( dt == stk::Stk::STK_FLOAT32 ) { return "float32"; }
    	else if( dt == stk::Stk::STK_FLOAT64 ) { return "float64"; }
		else {
			EmitError( "Unknown data type" );
		    return QString();
		}
    }
	stk::Stk::StkFormat ConvertFormat( const QString& dt ) const {
      	if( dt == "sint8"  ) { return stk::Stk::STK_SINT8; }
    	else if( dt == "sint16" ) { return stk::Stk::STK_SINT16; }
    	else if( dt == "sint24" ) { return stk::Stk::STK_SINT24; }
    	else if( dt == "sint32" ) { return stk::Stk::STK_SINT32; }
    	else if( dt == "float32" ) { return stk::Stk::STK_FLOAT32; }
    	else if( dt == "float64" ) { return stk::Stk::STK_FLOAT64; }
		else {
			EmitError( "Unknown data type: " + dt );
		    return stk::Stk::StkFormat();
		}
    }
    RtAudio::StreamParameters ConvertStreamParameters( const QVariantMap& pm ) const {
        RtAudio::StreamParameters params;
        unsigned deviceId = 1;
        unsigned nChannels = 1;		
        unsigned channelOffset = 0;
		if( pm.contains( "deviceId" ) ) deviceId = pm[ "deviceId" ].toUInt();
        if( pm.contains( "numChannels" ) ) nChannels = pm[ "numChannels" ].toUInt();
        if( pm.contains( "channelOffset" ) ) channelOffset = pm[ "channelOffset" ].toUInt();
        params.deviceId = deviceId;
        params.nChannels = nChannels;		
        params.firstChannel = channelOffset;
        return params;  
    }
	RtAudioStreamFlags ConvertStreamFlags( const QStringList& ol ) const {
		RtAudioStreamFlags options = 0;
		for( QStringList::const_iterator i = ol.begin(); i != ol.end(); ++i ) {
		    if( *i == "minimize-latency" ) options |= RTAUDIO_MINIMIZE_LATENCY;
			else if( *i == "non-interleaved" ) options |= RTAUDIO_NONINTERLEAVED;
			else if( *i == "hog-device" ) options |= RTAUDIO_HOG_DEVICE;
			else if( *i == "realtime" ) options |= RTAUDIO_SCHEDULE_REALTIME;
			else if( *i == "alsa-default" ) options |= RTAUDIO_ALSA_USE_DEFAULT;
			else {
				emit error( "Unknown option: " + *i );
			}
		}
		return options;
	}
	RtAudio::StreamOptions ConvertStreamOptions( const QVariantMap& om ) {
		RtAudio::StreamOptions options;
		if( om.contains( "flags" ) ) options.flags = ConvertStreamFlags( om[ "flags" ].toStringList() );
		if( om.contains( "numBuffers" ) ) options.numberOfBuffers = om[ "numBuffers" ].toUInt();
		if( om.contains( "streamName" ) ) options.streamName = om[ "numBuffers" ].toString().toStdString();
		if( om.contains( "priority" ) ) options.priority = om[ "priority" ].toInt();
		return options;
	}
	stk::StkFloat ComputeScaling( const QString& dt ) const {
	    if( dt == "sint8"  )       { return 1/stk::StkFloat( 0x7f ); }
	    else if( dt == "sint16" )  { return 1/stk::StkFloat( 0x7fff ); }
		else if( dt == "sint24" )  { return 1/stk::StkFloat( 0x7fffff ); }
		else if( dt == "sint32" )  { return 1/stk::StkFloat( 0x7fffffff ); }
		else if( dt == "float32" ) { return 1; }
		else if( dt == "float64" ) { return 1; }
		else {
		    EmitError( "Unknown data type: " + dt );
			return 0;
		}
	}
private:
	mutable RtAudio adc_;
    mutable RtAudioStreamFlags inputDataType_;
    mutable RtAudioStreamFlags outputDataType_;
	QVariantList output_;   
	QVariantList input_;
	unsigned bufferSize_;
	RtAudioStreamStatus status_;
	unsigned inputChannels_;
	unsigned outputChannels_;
	QObject* userData_;
};

Q_EXPORT_PLUGIN2( RtAudio, RtAudioPlugin )
