//#SRCHEADER
#include <stdexcept>


#include "LocoContext.h"
#include "LocoGUI.h"

#include "LocoMainWindow.h"
#include "LocoWidgetWrapper.h"

#include "LocoLayout.h"
#include "LocoWidget.h"

#ifdef LOCO_WKIT
#include "LocoWebWindow.h"
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
    } else

    //will be replaced with an IGUIFactory class
    //if support for different Widget sets is needed
#ifdef LOCO_WKIT
    if( name == "WebWindow" ) {
        WebWindow* wv = new WebWindow();
        if( !GetContext()->GetNetworkAccessManager() ) throw std::runtime_error( "NULL Network Access Manager" );
        wv->SetNetworkAccessManager( GetContext()->GetNetworkAccessManager() );
        QVariant obj = GetContext()->AddObjToJSContext( wv );
        return obj;
    } 
#endif 
    if( name == "WidgetWrapper" ) {
        WidgetWrapper *ww = new WidgetWrapper;
        QVariant obj = GetContext()->AddObjToJSContext( ww );
        return obj;      
    } else if( name == "Widget" ) {
        Widget *ww = new Widget;
        QVariant obj = GetContext()->AddObjToJSContext( ww );
        return obj;        
    } else if( name == "VBoxLayout" ) {
        VBoxLayout *l = new VBoxLayout;
        QVariant obj = GetContext()->AddObjToJSContext( l  );
        return obj;        
    } else if( name == "HBoxLayout" ) {
        HBoxLayout *l = new HBoxLayout;
        QVariant obj = GetContext()->AddObjToJSContext( l  );
        return obj;        
    } else {
        error( "GUI object \"" + name + "\" not recognized" );
        return QVariant();
    }
}

}
