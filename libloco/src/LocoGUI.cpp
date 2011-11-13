//#SRCHEADER
#include <stdexcept>

#include "LocoContext.h"
#include "LocoGUI.h"

#include "LocoMainWindow.h"

#ifdef LOCO_WKIT
#include "LocoWebWindow.h"
#include "LocoWebMainWindow.h"
#endif

namespace loco {

QVariant GUI::create( const QString& name, const QVariantMap& params ) const {
   
	if( GetContext() == 0 ) {
        error( "NULL Context" );
        return QVariant();
    }

    if( name == "MainWindow" ) {
        MainWindow* wv = new MainWindow();
        const bool NOT_OWNED_BY_JAVASCRIPT = false;
        QVariant obj = GetContext()->AddObjToJSContext( wv, NOT_OWNED_BY_JAVASCRIPT );
        return obj;
    }

    //will be replaced with an IGUIFactory class
    //if support for different Widget sets is needed
#ifdef LOCO_WKIT
    else if( name == "WebWindow" ) {
        WebWindow* wv = new WebWindow();
		if( !GetContext()->GetNetworkAccessManager() ) throw std::runtime_error( "NULL Network Access Manager" );
		wv->SetNetworkAccessManager( GetContext()->GetNetworkAccessManager() );
        QVariant obj = GetContext()->AddObjToJSContext( wv );
        return obj;
    } else  if( name == "WebMainWindow" ) {
        WebMainWindow* wv = new WebMainWindow();
		if( !GetContext()->GetNetworkAccessManager() ) throw std::runtime_error( "NULL Network Access Manager" );
		wv->SetNetworkAccessManager( GetContext()->GetNetworkAccessManager() );
        const bool NOT_OWNED_BY_JAVASCRIPT = false;
        QVariant obj = GetContext()->AddObjToJSContext( wv, NOT_OWNED_BY_JAVASCRIPT );
        return obj;
    }
    else {
        error( "GUI object \"" + name + "\" not recognized" );
        return QVariant();
    }
#else
    error( "GUI object \"" + name + "\" not recognized - QWebKit required" );
    return QVariant();
#endif

}

}
