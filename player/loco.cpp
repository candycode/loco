//#SRCHEADER

#include <iostream>
#include <exception>
#include "LocoApp.h"
#include "LocoConsole.h"
#include "LocoFileSystem.h"
#include "LocoSystem.h"
#include "LocoObjectInfo.h"
#include "LocoGUI.h"

//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	int ret = -1;
	try {
        QPointer< loco::ObjectInfo > i = new loco::ObjectInfo;
        i->SetName( "Loco test app" );
        QStringList sl; sl << "1" << "0";
        i->SetVersion( sl );
        i->SetLicense( "Freeware" );
        i->SetAuthor( "Ugo" );
        i->SetVendor( "Ugo" );
        i->SetUrl( "http://locojs.net" );   
		loco::App app( argc, argv, i );
		app.AddModuleToJS( new loco::Console );
		app.AddModuleToJS( new loco::FileSystem );
		app.AddModuleToJS( new loco::System );
        app.AddModuleToJS( new loco::GUI );
		app.AddContextToJS();
		app.SetAllowFileAccess( true );
		app.SetFilterFileAccess( false );
		ret = app.Execute();
	} catch( const std::exception& e ) {
		std::cerr << e.what() << std::endl;
	}
	return ret;
}
