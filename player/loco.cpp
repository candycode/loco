//#SRCHEADER

#include <iostream>
#include <exception>
#include "LocoApp.h"
#include "LocoConsole.h"
#include "LocoFileSystem.h"
#include "LocoSystem.h"

//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	int ret = -1;
	try {
		loco::App app( argc, argv );
		app.AddModuleToJS( new loco::Console );
		app.AddModuleToJS( new loco::FileSystem );
		app.AddModuleToJS( new loco::System );
		app.AddContextToJS();
		ret = app.Execute();
	} catch( const std::exception& e ) {
		std::cerr << e.what() << std::endl;
	}
	return ret;
}
