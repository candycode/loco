////////////////////////////////////////////////////////////////////////////////
//Copyright (c) 2012, Ugo Varetto
//All rights reserved.
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions are met:
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder nor the
//      names of its contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//DISCLAIMED. IN NO EVENT SHALL UGO VARETTO BE LIABLE FOR ANY
//DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////

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
