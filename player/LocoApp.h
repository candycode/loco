#pragma once
//SRCHEADER

#include <stdexcept>

#include <QString>
#include <QStringList>
#include <QMap>
#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebFrame>
#include "CmdLine.h"
#include "LocoObject.h"
#include "LocoContext.h"
#include "LocoNetworkAccessManager.h"
#include "LocoFileAccessManager.h"                                  
                                    

namespace loco {

typedef QMap< QString, Object* > ModuleMap;
typedef QVariantMap CMDLine; //using QVariantMap allows to pass the command line as a json object

class App {
public:
	App( int argc, char** argv ) : app_( argc, argv ),
		defaultScript_( ":/loco/main" ) { //read from embedded resource; 'main' is an alias for 'main.loco' file

	    // 1 - Read code to execute
        static const bool DO_NOT_REPORT_UNKNOWN_PARAMS = false;
        static const bool OPTIONAL = true;
        CmdLine cl( DO_NOT_REPORT_UNKNOWN_PARAMS );
        cmdLine_ = ParsedCommandsToCMDLine( cl.ParseCommandLine( argc, argv ) );
		//ctx_.setParent( this );
		helpText_ = "Usage: loco --script <javascript file>\n"
                    "       If no script file is specified the default "
                    "'main.loco' is used";
        wp_.setNetworkAccessManager( &netAccess_ );
        ctx_.SetNetworkAccessManager( &netAccess_ );
        ctx_.SetFileAccessManager( &fileAccess_ );
	}
    
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

    void SetAllowFileAccess( bool fa ) { fileAccess_.SetAllowFileAccess( fa ); }

    void SetFilterFileAccess( bool fr ) { fileAccess_.SetFilterAccess( fr ); }

    void SetFileRuleFormat( QRegExp::PatternSyntax ps ) { fileAccess_.SetRxPatternSyntax( ps ); }

    void AddAllowFileRule( const QRegExp& rx, QIODevice::OpenMode mode ) { fileAccess_.AddAllowRule( rx, mode ); }

    void AddDenyFileRule( const QRegExp& rx, QIODevice::OpenMode mode ) { fileAccess_.AddDenyRule( rx, mode ); }

    int Execute() {
		
        QString scriptFileName = defaultScript_; 
		if( cmdLine_[ " " ].toStringList().size() > 1 ) {
            scriptFileName = *( ++( cmdLine_[ " " ].toStringList().begin() ) );
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
	    wf_ = wp_.mainFrame();    
	    ctx_.Init( wf_, &app_, cmdLine_ );

	    // 3 - execute
	    execResult_ = ctx_.Eval( jscriptCode );
		if( execResult_.isValid() && execResult_.type() == QVariant::Int ) return execResult_.toInt();
		return 0;
	}
private:
	CMDLine ParsedCommandsToCMDLine( const CmdLine::ParsedEntries& cmd ) {
	    CMDLine cl;
	    for( CmdLine::ParsedEntries::const_iterator i = cmd.begin(); i != cmd.end(); ++i ) {
		    QStringList sl;
		    typedef std::vector< std::string > VS;
		    const VS& vs = i->second;
		    for( VS::const_iterator v = vs.begin(); v != vs.end(); ++v ) {
			    sl << QString( v->c_str() );
		    }
		    cl[ i->first.c_str() ] = sl;
	    }
	    return cl;
    }
private:
	CMDLine cmdLine_;
	QApplication app_;
	QWebPage wp_;
	QWebFrame* wf_;
	Context ctx_;
	QString defaultScript_;
	QString helpText_;
	QVariant execResult_;
    NetworkAccessManager netAccess_;
    FileAccessManager fileAccess_;
};
}
