#pragma once
//SRCHEADER

#include <stdexcept>

#include <QString>
#include <QStringList>
#include <QMap>
#include <QListIterator>
#include <QPointer>

#include "LocoObject.h"
#include "LocoContext.h"
#include "LocoNetworkAccessManager.h"
#include "LocoFileAccessManager.h"     
#include "LocoObjectInfo.h" //reusing this for app info
#include "LocoQtApp.h"

namespace loco {

typedef QMap< QString, Object* > ModuleMap;
typedef QStringList CMDLine;

class App : public QObject {
    Q_OBJECT
private:
    enum RuleType { NetDeny, NetAllow, FileDeny, FileAllow };
public:
	App( LocoQtApp& app, int argc, char** argv, ObjectInfo* oi ) : app_( app ),
        jsInterpreter_( 0 ),
	    defaultScript_( ":/loco/main" ),
	    info_( oi ),
#ifdef LOCO_GUI
	    startEventLoop_( true ) { //read from embedded resource; 'main' is an alias for 'main.loco' file
#else
		startEventLoop_( false ) {
#endif
        info_->setParent( this );
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
		
	}

	bool GetEventLoopEnable() const { return startEventLoop_; }

	void SetEventLoopEnable( bool yes ) { startEventLoop_ = yes; }

	void SetInterpreter( IJSInterpreter* i ) {
        if( jsInterpreter_ != 0 && i == jsInterpreter_ ) return;
        if( jsInterpreter_ != 0 ) jsInterpreter_->deleteLater();
        jsInterpreter_ = i;
        jsInterpreter_->setParent( this ); // app owns interpreter
    }
    
	void AddModuleToJS( Object* obj ) {
		ctx_.AddJSStdObject( obj ); // context owns module
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

    void AddAllowFileRule( const QRegExp& rx, QIODevice::OpenMode mode = QIODevice::ReadWrite ) { fileAccess_.AddAllowRule( rx, mode ); }

    void AddDenyFileRule( const QRegExp& rx, QIODevice::OpenMode mode = QIODevice::ReadWrite ) { fileAccess_.AddDenyRule( rx, mode ); }

    void SetSignalNetAccessDenied( bool yes ) { netAccess_.SetSignalAccessDenied( yes ); }

    void SetDefaultNetUrl( const QString& durl ) { netAccess_.SetDefaultUrl( durl ); }

    void SetDefaultScript( const QString& ds ) { defaultScript_ = ds; }

    void ParseCommandLine() {
        if( std::find( cmdLine_.begin(), cmdLine_.end(), "--no-event-loop" ) != cmdLine_.end() ||
    	    std::find( cmdLine_.begin(), cmdLine_.end(), "-nl" ) != cmdLine_.end() ) startEventLoop_ = false;
        QStringList::const_iterator i;
        // net deny
        i = std::find( cmdLine_.begin(), cmdLine_.end(), "--net-deny" );
        if( i == cmdLine_.end() ) i = std::find( cmdLine_.begin(), cmdLine_.end(), "-nd" );
        if( i != cmdLine_.end() ) {
        	SetAllowNetAccess( true );
        	SetFilterNetRequests( true );
        	ReadRules( *( ++i ), NetDeny );
        }
        // net allow
        i = std::find( cmdLine_.begin(), cmdLine_.end(), "--net-allow" );
        if( i == cmdLine_.end() ) i = std::find( cmdLine_.begin(), cmdLine_.end(), "-na" );
        if( i != cmdLine_.end() ) {
        	SetAllowNetAccess( true );
        	SetFilterNetRequests( true );
            ReadRules( *( ++i ), NetAllow );
        }
        // file deny
        i = std::find( cmdLine_.begin(), cmdLine_.end(), "--file-deny" );
        if( i == cmdLine_.end() ) i = std::find( cmdLine_.begin(), cmdLine_.end(), "-fd" );
        if( i != cmdLine_.end() ) {
        	SetAllowFileAccess( true );
        	SetFilterFileAccess( true );
        	ReadRules( *( ++i ), FileDeny );
        }
        // file allow
        i = std::find( cmdLine_.begin(), cmdLine_.end(), "--file-allow" );
        if( i == cmdLine_.end() ) i = std::find( cmdLine_.begin(), cmdLine_.end(), "-fa" );
        if( i != cmdLine_.end() ) {
        	SetAllowFileAccess( true );
        	SetFilterFileAccess( true );
        	ReadRules( *( ++i ), FileAllow );
        }
        // full file access
        i = std::find( cmdLine_.begin(), cmdLine_.end(), "--file" );
        if( i == cmdLine_.end() ) i = std::find( cmdLine_.begin(), cmdLine_.end(), "-f" );
        if( i != cmdLine_.end() ) {
        	QString f = *( ++i );
            if( f == "on" ) {
                SetAllowFileAccess( true );
                SetFilterFileAccess( false );
            } else if( f == "off" ) SetAllowFileAccess( false );
            else throw std::runtime_error( "Unknown file access option " + f.toStdString() );
        }
        // full network access
        i = std::find( cmdLine_.begin(), cmdLine_.end(), "--network" );
        if( i == cmdLine_.end() ) i = std::find( cmdLine_.begin(), cmdLine_.end(), "-n" );
        if( i != cmdLine_.end() ) {
        	QString n = *( ++i );
            if(n == "on" ) {
        	    SetAllowNetAccess( true );
                SetFilterNetRequests( false );
            } else if( n == "off" ) SetAllowFileAccess( false );
            else throw std::runtime_error( "Unknown network access option " + n.toStdString() );
        }
    }

    int Execute( bool forceDefault = false ) {
        try {		
            QString scriptFileName = defaultScript_;
            if( !forceDefault ) {
            	// no rbegin/rend, cannot use find_if
            	QListIterator<QString> li( cmdLine_ );
            	QString v;
            	for( li.toBack(); li.hasPrevious(); v = li.previous() ) {
					if( v.toLower().endsWith( ".js" )  ||
						v.toLower().endsWith( "loco" ) ||
						v.toLower().endsWith( ".ljs" ) ) break;
            	}
            	if( li.hasPrevious() ) scriptFileName = v;
            }
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
	        ctx_.Init( jsInterpreter_, &app_, cmdLine_ );

	        // 3 - execute
	        execResult_ = ctx_.Eval( jscriptCode );
            int execResult = 0;
	        if( startEventLoop_ ) execResult = app_.exec();
		    if( execResult_.isValid() && execResult_.type() == QVariant::Int ) return execResult_.toInt();
		    return execResult;
        } catch( const std::exception& e ) {
            emit OnException( e.what() );
            std::cerr << e.what() << std::endl;
            return -1;
        }
        return -1;
	}

    void ConfigNetAccessFromFile( const QString& deny,
    		                      const QString& allow ) {
    	ReadRules( deny, NetDeny );
    	ReadRules( allow, NetAllow );
    }

    void ConfigFileAccessFromFile( const QString& deny,
    		                       const QString& allow ) {
    	ReadRules( deny, FileDeny );
    	ReadRules( allow, FileAllow );
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
    void ReadRules( const QString& fname, RuleType target ) {
    	 QFile f( fname );
    	 if( !f.exists() ) {
    	     throw std::runtime_error( "file " + fname.toStdString() + " does not exist" );
    	 }
    	 if( !f.open( QIODevice::ReadOnly ) ) {
    	     throw std::runtime_error( "Cannot open file: " + fname.toStdString() );
    	 }
    	 while( !f.atEnd() ) {
    	 	 QString l = f.readLine();
    		 if( l.isEmpty() || l.startsWith( '#' ) ) continue;
    		 switch( target ) {
    		 case NetDeny: AddDenyNetRule( QRegExp( l ) );
    		               break;
    		 case NetAllow: AddAllowNetRule( QRegExp( l ) );
    		     		    break;
    		 case FileDeny: AddDenyFileRule( QRegExp( l ) );
    		     		    break;
    		 case FileAllow: AddAllowFileRule( QRegExp( l ) );
    		     		     break;
    		 default: break;
    		 }
    	 }
    }

private:
	CMDLine cmdLine_;
	LocoQtApp& app_;
	IJSInterpreter* jsInterpreter_;
	Context ctx_;
	QString defaultScript_;
    QPointer< ObjectInfo > info_;
	QString helpText_;
	QVariant execResult_;
    NetworkAccessManager netAccess_;
    FileAccessManager fileAccess_;
    bool startEventLoop_;
};
}
