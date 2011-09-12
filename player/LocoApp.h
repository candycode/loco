#pragma once
//SRCHEADER

#include <QString>
#include <QStringList>
#include <QMap>
#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebFrame>
#include "CmdLine.h"
#include "LocoObject.h"
#include "LocoContext.h"                             
                                    

namespace loco {

typedef QMap< QString, Object* > ModuleMap;
typedef QVariantMap CMDLine; //using QVariantMap allows to pass the command line as a json object

class App {
public:
	App( int argc, char** argv ) : app_( argc, argv ),
		defaultScript_( "main.loco" ) {

	    // 1 - Read code to execute
        static const bool DO_NOT_REPORT_UNKNOWN_PARAMS = false;
        static const bool OPTIONAL = true;
        CmdLine cl( DO_NOT_REPORT_UNKNOWN_PARAMS );
        cl.Add( "Script to execute", "script", "s", std::make_pair( 1, 1 ), OPTIONAL );
        cmdLine_ = ParsedCommandsToCMDLine( cl.ParseCommandLine( argc, argv ) );
		//ctx_.setParent( this );
		helpText_ = "Usage: loco --script <javascript file>\n"
                    "       If no script file is specified the default "
                    "'main.loco' is used";
	}
    
	void AddModuleToJS( Object* obj ) {
		ctx_.AddJSStdObject( obj );
	}

	void AddContextToJS() { //adds context to the list of objects accessible from javascript
	    ctx_.AddContextToJS();
	}

    const QVariant& GetResult() const { return execResult_; }

	// set the custom network access manager to be used for ALL network connections in javascript
	// contexts; substitute in QWebFrame instances!
    void SetCustomNetworkAccesManager( QNetworkAccessManager* nam );

	int Execute() {
		
        const QString scriptFileName = cmdLine_.find( "script" ) == cmdLine_.end() ? defaultScript_ 
			: cmdLine_[ "script" ].toStringList().front();
        QFile scriptFile( scriptFileName );
        if( !scriptFile.exists() ) {
			std::cout << helpText_.toStdString() << std::endl;
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
};
}
