//#SRCHEADER
#include <stdexcept>

#include "LocoContext.h"
#include "LocoGUI.h"
#include "LocoWebWindow.h"
#include "LocoWebMainWindow.h"

namespace loco {

QVariant GUI::create( const QString& name, const QVariantMap& params ) const {
   
	if( GetContext() == 0 ) {
        error( "NULL Context" );
        return QVariant();
    }

    //will be replaced with an IGUIFactory class
    //if support for different Widget sets is needed

    if( name == "WebWindow" ) {
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
    } else {
        error( "GUI object \"" + name + "\" not recognized" );
        return QVariant();
    }
}

}
