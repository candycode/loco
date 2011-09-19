//#SRCHEADER

#include <iostream>
#include <stdexcept>

#include <QSharedPointer>

#include "LocoApp.h"
#include "LocoConsole.h"
#include "LocoFileSystem.h"
#include "LocoSystem.h"
#include "LocoObjectInfo.h"
#ifdef LOCO_WKIT
#include "LocoWebKitJSCore.h"
#else
#include "LocoDefaultJS.h"
#endif

#ifdef LOCO_GUI
#include "LocoGUI.h"
#endif

#include "LocoDefaultJS.h"


//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	int ret = -1;
	try {
        QSharedPointer< loco::ObjectInfo > i( new loco::ObjectInfo );
        i->SetName( "Loco test app" );
        QStringList sl; sl << "1" << "0";
        i->SetVersion( sl );
        i->SetLicense( "Freeware" );
        i->SetAuthor( "Ugo" );
        i->SetVendor( "Ugo" );
        i->SetUrl( "http://locojs.net" );   
		loco::App app( argc, argv, i );
		//WARNING: interpreter MUST BE CREATED AFTER APP if not
		//a ERROR IS REPORTED (QWidgets must be created after QApplication
		//and in the case of the QtWebKit jscore interpreter the interpreter
		//is inside a class that has a dependency on QWidget.
#ifdef LOCO_WKIT
		app.SetInterpreter( QSharedPointer< loco::WebKitJSCore >( new loco::WebKitJSCore ) );
#else
		app.SetInterpreter( QSharedPointer< loco::DefaultJS >( new loco::DefaultJS ) );
#endif
		app.AddModuleToJS( new loco::Console );
		app.AddModuleToJS( new loco::FileSystem );
		app.AddModuleToJS( new loco::System );
#ifdef LOCO_GUI
        app.AddModuleToJS( new loco::GUI );
#endif
		app.AddContextToJS();
		app.SetAllowFileAccess( true );
		app.SetFilterFileAccess( false );
		app.SetAllowNetAccess( true );
		app.SetFilterNetRequests( false );
		ret = app.Execute();
	} catch( const std::exception& e ) {
		std::cerr << e.what() << std::endl;
	}
	return ret;
}
