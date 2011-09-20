//#SRCHEADER
#include <stdexcept>

#include "LocoContext.h"
#include "LocoGUI.h"
#include "LocoWebWindow.h"

namespace loco {

QVariant GUI::create( const QString& name, const QVariantMap& params ) const {
    if( GetContext() == 0 ) {
        error( "NULL Context" );
        return QVariant();
    }

    //will be replaced with an IGUIFactory class
    //if we need to support other Widget type

    if( name == "WebWindow" ) {
        WebWindow* wv = new WebWindow();
		if( !GetContext()->GetNetworkAccessManager() ) throw std::runtime_error( "NULL context" );
		wv->SetNetworkAccessManager( GetContext()->GetNetworkAccessManager() );
        QVariant obj = GetContext()->AddObjToJSContext( wv );
		
        return obj;
    } else {
        error( "GUI object \"" + name + "\" not recognized" );
        return QVariant();
    }
}

//QVariantMap GUI::colorDialog( const QString& title = "", const QVariantMap& c = QVariantMap() ) {
//    if( !c.isEmpty() ) {
//        QColor color( c["r"].toInt(), c["g"].toInt(), c["b"].toInt(), c["a"].toInt() );
//        color = QColorDialog::getColor( color, 0, title, QColorDialog::ShowAlphaChannel );
//    }
//}


}
