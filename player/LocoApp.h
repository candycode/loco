#pragma once
//SRCHEADER

#include <stdexcept>

#include <QString>
#include <QStringList>
#include <QMap>

#include <QSharedPointer>

#include "LocoObject.h"
#include "LocoContext.h"
#include "LocoNetworkAccessManager.h"
#include "LocoFileAccessManager.h"     
#include "LocoObjectInfo.h" //reusing this for app info
                                    

namespace loco {

typedef QMap< QString, Object* > ModuleMap;
typedef QStringList CMDLine;

class App : public QObject {
    Q_OBJECT
public:
	App( int argc, char** argv, QSharedPointer< ObjectInfo > oi ) : app_( argc, argv ),
	    defaultScript_( ":/loco/main" ),
	    info_( oi ),
#ifdef LOCO_GUI
	    startEventLoop_( true ) { //read from embedded resource; 'main' is an alias for 'main.loco' file
#else
		startEventLoop_( false ) {
#endif

	    // 1 - Read code to execute
        static const bool DO_NOT_REPORT_UNKNOWN_PARAMS = false;
        static const bool OPTIONAL = true;
        ctx_.SetNetworkAccessManager( &netAccess_ );
        ctx_.SetFileAccessManager( &fileAccess_ );
        connect( this, SIGNAL( OnException( const QString&  ) ),
                 &ctx_, SLOT( OnExternalError( const QString& ) ) );
        app_.setOrganizationName( info_->vendor() );
        app_.setOrganizationDomain( info_->url() );
        app_.setApplicationVersion( info_->version().join( "," ) );
        app_.setApplicationName( info_->name() ); 
        ctx_.SetAppInfo( info_ );
        cmdLine_ = QCoreApplication::arguments();
        if( std::find( cmdLine_.begin(), cmdLine_.end(), "--no-event-loop" ) != cmdLine_.end() ||
        	std::find( cmdLine_.begin(), cmdLine_.end(), "-nl" ) != cmdLine_.end() ) startEventLoop_ = false;
	}

	bool GetEventLoopEnable() const { return startEventLoop_; }

	void SetEventLoopEnable( bool yes ) { startEventLoop_ = yes; }

	void SetInterpreter( QSharedPointer< IJSInterpreter > i ) { jsInterpreter_ = i; }
    
	void AddModuleToJS( Object* obj ) {
		ctx_.AddJSStdObject( obj );
	}

	void AddContextToJS() { //adds context to the list of objects accessible from javascript
	    ctx_.AddContextToJS();
	}
    
    const QVariant& GetResult() const { return execResult_; }
	
    void SetAllowNetAccess( bool na ) { netAccess_.SetAllowNetAccess( na ); }

    void SetFilterNetRequests( bool fr ) { netAccess_.SetFilterRequests( fr ); }

    void SetNetRuleFormat( QRegExp::PatternSyntax ps ) { netAccess_.SetRxPatternSyntax( ps ); }

    void AddAllowNetRule( const QRegExp& rx ) { netAccess_.AddAllowRule( rx ); }

    void AddDenyNetRule( const QRegExp& rx ) { netAccess_.AddDenyRule( rx ); }

    void AddNetUrlMapping( const QRegExp& rx, const QString& url ) { netAccess_.AddRedirMapping( rx, url ); }

    void SetAllowFileAccess( bool fa ) { fileAccess_.SetAllowFileAccess( fa ); }

    void SetFilterFileAccess( bool fr ) { fileAccess_.SetFilterAccess( fr ); }

    void SetFileRuleFormat( QRegExp::PatternSyntax ps ) { fileAccess_.SetRxPatternSyntax( ps ); }

    void AddAllowFileRule( const QRegExp& rx, QIODevice::OpenMode mode ) { fileAccess_.AddAllowRule( rx, mode ); }

    void AddDenyFileRule( const QRegExp& rx, QIODevice::OpenMode mode ) { fileAccess_.AddDenyRule( rx, mode ); }

    int Execute() {
        try {		
            QString scriptFileName = defaultScript_;
            const QString lastCmd = cmdLine_.back();
            if( !lastCmd.startsWith( "-" ) ) scriptFileName = lastCmd;

            QFile scriptFile( scriptFileName );
            if( !scriptFile.exists() ) {
			    throw std::runtime_error( "file " + scriptFileName.toStdString() + " does not exist" );
        	    return 1;
            }
            if( !scriptFile.open( QIODevice::ReadOnly ) ) {
                throw std::runtime_error( "Cannot open file: " + scriptFileName.toStdString() );
            }
            QString jscriptCode = scriptFile.readAll();
            if( scriptFile.error() != QFile::NoError ) {
        	    throw std::runtime_error( scriptFile.errorString().toStdString() );
            }
            scriptFile.close();
            if( jscriptCode.isEmpty() ) throw std::logic_error( "Empty javascript source file" );
                                                
            // 2 - Create run-time environment                             
	        ctx_.Init( jsInterpreter_.data(), &app_, cmdLine_ );

	        // 3 - execute
	        execResult_ = ctx_.Eval( jscriptCode );
            int execResult = 0;
	        if( startEventLoop_ ) execResult = app_.exec();
		    if( execResult_.isValid() && execResult_.type() == QVariant::Int ) return execResult_.toInt();
		    else return execResult;
        } catch( const std::exception& e ) {
            emit OnException( e.what() );
            std::cerr << e.what() << std::endl;
            return -1;
        }
	}

    void AddNameFilterMapping( const QRegExp& rx, const QStringList& filterIds ) {
        ctx_.AddNameFilterMapping( rx, filterIds );
    }

    void RemoveNameFilterMapping( const QRegExp& rx ) {
        ctx_.RemoveNameFilterMapping( rx );
    }   
   
    void PreloadFilter( const QString& id, const QString& uri ) {
        ctx_.LoadFilter( id, uri );
    }

    void PreloadScriptFilter( const QString& id, const QString& uri ) {
        ctx_.AddScriptFilter( id, uri );
    }

    void PreloadObject( const QString& uri ) {
        const bool PERSISTENT = true;
        ctx_.LoadObject( uri, PERSISTENT );
    }

signals:
    void OnException( const QString& );

private:
	CMDLine cmdLine_;
	LocoQtApp app_;
	QSharedPointer< IJSInterpreter > jsInterpreter_;
	Context ctx_;
	QString defaultScript_;
    QSharedPointer< ObjectInfo > info_;
	QString helpText_;
	QVariant execResult_;
    NetworkAccessManager netAccess_;
    FileAccessManager fileAccess_;
    bool startEventLoop_;
};
}
