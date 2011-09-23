//#SRCHEADER

#include <iostream>
#include <stdexcept>

#include <QSharedPointer>

#include "LocoApp.h"

#ifdef LOCO_CONSOLE
#include "LocoConsole.h"
#endif
#ifdef LOCO_FSYSTEM
#include "LocoFileSystem.h"
#endif
#ifdef LOCO_SYSTEM
#include "LocoSystem.h"
#endif
#include "LocoObjectInfo.h"
#ifdef LOCO_WKIT
#include "LocoWebKitJSCore.h"
#else
#include "LocoDefaultJS.h"
#endif
#ifdef LOCO_GUI
#include "LocoGUI.h"
#endif

#include <LocoAppConfig.h>


using namespace loco;

//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	int ret = -1;
	try {
        QSharedPointer< loco::ObjectInfo > i( new loco::ObjectInfo );
        QStringList sl = LOCO_APP_VERSION.split(".");
        i->SetName( LOCO_APP_NAME )
          .SetVersion( sl )
          .SetLicense( LOCO_APP_LICENSE )
          .SetAuthor( LOCO_APP_AUTHOR )
          .SetCopyright( LOCO_APP_COPYRIGHT )
          .SetVendor( LOCO_APP_VENDOR )
          .SetUrl( LOCO_APP_URL );
		loco::App app( argc, argv, i );
		//WARNING: interpreter MUST BE CREATED AFTER APP if not
		//a ERROR IS REPORTED (QWidgets must be created after QApplication
		//and in the case of the QtWebKit jscore interpreter the interpreter
		//is inside a class that has a dependency on QWidget.
#ifdef LOCO_WKIT
		app.SetInterpreter( QSharedPointer< WebKitJSCore >( new WebKitJSCore ) );
#else
		app.SetInterpreter( QSharedPointer< DefaultJS >( new DefaultJS ) );
#endif

#ifdef LOCO_CONSOLE
		app.AddModuleToJS( new Console );
#endif
#ifdef LOCO_FSYSTEM
		app.AddModuleToJS( new FileSystem );
		app.SetAllowFileAccess( true );
		app.SetFilterFileAccess( false );
#endif
#ifdef LOCO_SYSTEM
		app.AddModuleToJS( new System );
#endif
#ifdef LOCO_GUI
        app.AddModuleToJS( new GUI );
#endif
#ifdef LOCO_CONTEXT
		app.AddContextToJS();
#endif

#ifdef LOCO_NETWORK
		app.SetAllowNetAccess( true );
		app.SetFilterNetRequests( false );
#endif
		app.ParseCommandLine();
		ret = app.Execute();
	} catch( const std::exception& e ) {
		std::cerr << e.what() << std::endl;
	}
	return ret;
}
