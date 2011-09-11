//#SRCHEADER

#include <iostream>
#include <vector>
#include <map>
#include <string>

#include <QApplication>
#include <QString>
#include <QVariant>
#include <QMap>
#include <QStringList>
#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebFrame> 
#include <QFile>

#include "LocoContext.h"

#include "CmdLine.h"


static const QString DEFAULT_BOOTSTRAP_NAME = "main.loco";

static const std::string HELP_TEXT = "Usage: loco --script <javascript file>\n"
                                     "       If no script file is specified the default "
                                     "'main.loco' is used";
                                    
                                    

typedef QVariantMap CMDLine; //using QVariantMap allows to pass the command line as a json object

typedef QMap< QString, LocoObject* > ModuleMap;

namespace loco {
class App {
public:
	App( int argc, char** argv ) : app_( argc, argv ) {
	    // 1 - Read code to execute
        static const bool DO_NOT_REPORT_UNKNOWN_PARAMS = false;
        static const bool OPTIONAL = true;
        CmdLine cl( DO_NOT_REPORT_UNKNOWN_PARAMS );
        cl.Add( "Script to execute", "script", "s", std::make_pair( 1, 1 ), OPTIONAL );
        cmdLine_ = cl.ParseCommandLine( argc, argv );
		ctx_.setParent( this );
	}
    
	void AddModule( LocoObject* obj ) {
		ctx_.AddJSStdObject( obj );
	}

	void AddContext() { //adds context to the list of objects accessible from javascript
	    ctx_.AddJSStdObject( *ctx_ );
	}

	// set the custom network access manager to be used for ALL network connections in javascript
	// contexts; substitute in QWebFrame instances!
    void SetCustomNetworkAccesManager( QNetworkAccessManager* nam );

	int Execute() {
		
        const QString scriptFileName = cmdLine_.find( "script" ) == pc.end() ? DEFAULT_BOOTSTRAP_NAME 
                                      : cmdLine[ "script" ][ 0 ].c_str();
        QFile scriptFile( scriptFileName );
        if( !scriptFile.exists() ) {
        	std::cout << HELP_TEXT << std::endl;
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
	    ctx_.eval( jscriptCode );       
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
};
}


//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	try {
		loco::App app( argc, argv );
		app.AddModule( new loco::Console );
		app.AddModule( new loco::FileSystem );
		app.AddModule( new loco::System );
		app.AddContext();
		ret = app.Execute();
	} catch( const std::exception& e ) {
		std::cerr << e.what() << std::endl;
	}
	return ret;
}
