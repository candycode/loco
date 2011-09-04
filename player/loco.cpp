//#SRCHEADER

#include <iostream>
#include <vector>
#include <map>
#include <string>

#include <QApplication>
#include <QString>
#include <QVariant>
#include <QMap>
#include <QStrinList>
#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebFrame> 
#include <QFile>

#include "LocoContext.h"

#include "CmdLine.h"


static const QString DEFAULT_BOOTSTRAP_NAME = "main.loco";

static const std::string HELP_TEXT = "Usage: loco --script <javascript file>\n"
                                     "       If no script file is specified the default "
                                     "'main.loco' is used";
                                    
                                    

typedef QVariantMap CMDLine;

//------------------------------------------------------------------------------
CMDLine ParsedCommandsToCMDLine( const CmdLine::ParsedCommands& cmd ) {
	CMDLine cl;
	for( CmdLine::ParsedCommands::const_iterator i = cmd.begin(); i != cmd.end(); ++i ) {
		QStringList sl;
		typedef std::vector< std::string > VS;
		const VS& vs = i->second();
		for( VS::const_iterator v = vs.begin(); v != vs.end(); ++v ) {
			sl << QString( *v->c_str() );
		}
		cl[ i->first().c_str() ] = sl;
	}
	return cl;
}

//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	try {
		// 1 - Read code to execute
        static const bool DO_NOT_REPORT_UNKNOWN_PARAMS = false;
        static const bool OPTIONAL = true;
        CmdLine cl( DO_NOT_REPORT_UNKNOWN_PARAMS );
        cl.Add( "Script to execute", "script", "s", std::make_pair( 1, 1 ), OPTIONAL );
        CmdLine::ParsedCommands pc = cl.ParseCommandLine( argc, argv );
        const QString scriptFileName = pc.find( "script" ) == pc.end() ? DEFAULT_BOOTSTRAP_NAME 
                                      : pc[ "script" ][ 0 ].c_str();
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
	    QApplication app( argc, argv );
	    
	    QWebPage wp;
	    QWebFrame* wf = wp.mainFrame();
	    loco::Context ctx( wf, &app, ParsedCommandsToCMDLine( pc ) );

	    // 3 - execute
	    QVariant ret = wf->evaluateJavaScript( jscriptCode );
        if( ctx.Error() ) {
        	std::cerr << ctx.lastError() << std::endl;
        	return 1;
        }
        if( ret.isValid() ) {
        	const QString r = ret.toString();
        	if( !r.isEmpty() ) std::cout << r.toStdString() << std::endl;
        }
        return 0; 
    } catch( const std::exception& e ) {
    	std::cerr << e.what() << std::endl;
    	return 1;
    }
}
