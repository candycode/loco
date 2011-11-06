//SRCHEADER
#include "LocoApp.h"

namespace loco {
App::App( LocoQtApp& app, int argc, char** argv, ObjectInfo* oi ) : app_( app ),
        jsInterpreter_( 0 ), defaultScript_( ":/loco/main" ), info_( oi ),
		scriptNameRX_( ".+\\.js$|.+\\.loco$|.+\\.ljs$" ), 
#ifdef LOCO_GUI
	    startEventLoop_( true ) { 
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
	connect( &ctx_, SIGNAL( JavaScriptConsoleMessage( const QString&, int, const QString& ) ),
			 this, SLOT( OnJavaScriptConsoleMessage( const QString&, int, const QString& ) ) );
    app_.setOrganizationName( info_->vendor() );
    app_.setOrganizationDomain( info_->url() );
    app_.setApplicationVersion( info_->version().join( "," ) );
    app_.setApplicationName( info_->name() );
#ifdef LOCO_GUI
    connect( &app_, SIGNAL( lastWindowClosed() ), &app_, SLOT( quit() ) );
#endif         
    ctx_.SetAppInfo( info_ );
    cmdLine_ = QCoreApplication::arguments();
	
}

void App::SetInterpreter( IJSInterpreter* i ) {
    if( jsInterpreter_ != 0 && i == jsInterpreter_ ) return;
    if( jsInterpreter_ != 0 ) jsInterpreter_->deleteLater();
    jsInterpreter_ = i;
    jsInterpreter_->setParent( this ); // app owns interpreter
 }
    

void App::ParseCommandLine() {
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

	
int App::Execute( bool forceDefault  ) {
    try {		
        scriptFileName_ = defaultScript_;
        if( !forceDefault ) {
        	const int si = cmdLine_.indexOf( scriptNameRX_ );
        	if( si >= 0 ) scriptFileName_ = cmdLine_[ si ];
        }
        QFile scriptFile( scriptFileName_ );
        if( !scriptFile.exists() ) {
		    throw std::runtime_error( "file " + scriptFileName_.toStdString() + " does not exist" );
    	    return -1;
        }
        if( !scriptFile.open( QIODevice::ReadOnly ) ) {
            throw std::runtime_error( "Cannot open file: " + scriptFileName_.toStdString() );
            return -1;
        }
        scriptFile.close();
        // execute
        execResult_ = ctx_.Insert( scriptFileName_ );
        int execResult = 0;
        if( startEventLoop_ ) execResult = app_.exec();
	    if( execResult_.isValid() && execResult_.type() == QVariant::Int ) return execResult_.toInt();
	    return execResult;
    } catch( const std::exception& e ) {
        emit OnException( e.what() );
		std::cerr << app_.applicationName().toStdString() << " - " << e.what() << std::endl;
#ifdef LOCO_GUI
		QMessageBox::critical( 0, app_.applicationName(), e.what() );
#endif
        return -1;
    }
    return -1;
}

//private slots:
void App::OnJavaScriptConsoleMessage( const QString& t, int l, const QString& s ) {
	const QString sid = s == "undefined" || s.isEmpty() ? scriptFileName_ : s;
	QString msg = QString("%1, line %2> %3").arg( sid ).arg( l ).arg( t );
	if( t.toLower().contains( "error" ) ) {	
		throw std::runtime_error( msg.toStdString() );
	}
}
//private:
void App::ReadRules( const QString& fname, RuleType target ) {
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

}
