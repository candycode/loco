//#SRCHEADER

#include <iostream>
#include <stdexcept>

#include <LocoApp.h>

#ifdef LOCOPLAY_CONSOLE
#include <LocoConsole.h>
#endif
#ifdef LOCOPLAY_FSYSTEM
#include <LocoFileSystem.h>
#endif
#ifdef LOCOPLAY_SYSTEM
#include <LocoSystem.h>
#endif
#include <LocoObjectInfo.h>
#ifdef LOCOPLAY_WKIT
#include <LocoWebKitJSCore.h>
#else
#include <LocoDefaultJS.h>
#endif
#ifdef LOCOPLAY_GUI
#include <LocoGUI.h>
#endif
#include <LocoQtApp.h>

#include <LocoAppConfig.h>



using namespace loco;

//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	int ret = -1;
	try {
        LocoQtApp qtApp( argc, argv );
        loco::ObjectInfo* i = new loco::ObjectInfo;
        QStringList sl = LOCO_APP_VERSION.split(".");
        i->SetName( LOCO_APP_NAME )
          .SetVersion( sl )
          .SetLicense( LOCO_APP_LICENSE )
          .SetAuthor( LOCO_APP_AUTHOR )
          .SetCopyright( LOCO_APP_COPYRIGHT )
          .SetVendor( LOCO_APP_VENDOR )
          .SetUrl( LOCO_APP_URL );
		loco::App app( qtApp, argc, argv, i );
		//WARNING: interpreter MUST BE CREATED AFTER APP if not
		//a ERROR IS REPORTED (QWidgets must be created after QApplication
		//and in the case of the QtWebKit jscore interpreter the interpreter
		//is inside a class that has a dependency on QWidget.
#ifdef LOCOPLAY_WKIT
		app.SetInterpreter( new WebKitJSCore ); // app owns interpreter
#else
		app.SetInterpreter( new DefaultJS );
#endif

#ifdef LOCOPLAY_CONSOLE
		app.AddModuleToJS( new Console );
#endif
#ifdef LOCOPLAY_FSYSTEM
		app.AddModuleToJS( new FileSystem );
		app.SetAllowFileAccess( true );
		app.SetFilterFileAccess( false );
#endif
#ifdef LOCOPLAY_SYSTEM
		app.AddModuleToJS( new System );
#endif
#ifdef LOCOPLAY_GUI
        app.AddModuleToJS( new GUI );
#endif
#ifdef LOCOPLAY_CONTEXT
		app.AddContextToJS();
#endif

#ifdef LOCOPLAY_NETWORK
		app.SetAllowNetAccess( true );
		app.SetFilterNetRequests( false );
#endif
		app.ParseCommandLine();
		ret = app.Execute();
	} catch( const std::exception& e ) {
		std::cerr << "locoplay: " << e.what() << std::endl;
	}
	return ret;
}
