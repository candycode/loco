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

#include <QMap>
#include <QString>

#include "../include/LocoWindowAttributes.h"


namespace loco {


namespace {
QMap< QString, Qt::WidgetAttribute >
AttrMap() {
    QMap< QString, Qt::WidgetAttribute > attrMap;
    attrMap[ "accept-drops"                 ] = Qt::WA_AcceptDrops;
    attrMap[ "delete-on- close"             ] = Qt::WA_DeleteOnClose;
    attrMap[ "disabled"                     ] = Qt::WA_Disabled; 
    attrMap[ "dont-show-on-screen"          ] = Qt::WA_DontShowOnScreen;
    attrMap[ "hover"                        ] = Qt::WA_Hover;
    attrMap[ "input-method-enabled"         ] = Qt::WA_InputMethodEnabled;
    attrMap[ "mac-no-click-through"         ] = Qt::WA_MacNoClickThrough;
    attrMap[ "mac-show-focus-rect"          ] = Qt::WA_MacShowFocusRect;
    attrMap[ "mac-normal-size"              ] = Qt::WA_MacNormalSize;
    attrMap[ "mac-smal-size"                ] = Qt::WA_MacSmallSize;
    attrMap[ "mac-mini-size"                ] = Qt::WA_MacMiniSize;
    attrMap[ "mac-variable-size"            ] = Qt::WA_MacVariableSize;
    attrMap[ "mac-brushed-metal"            ] = Qt::WA_MacBrushedMetal;
    attrMap[ "mouse-tracking"               ] = Qt::WA_MouseTracking;
    attrMap[ "no-mouse-propagation"         ] = Qt::WA_NoMousePropagation;
    attrMap[ "transparent-for-mouse-events" ] = Qt::WA_TransparentForMouseEvents;
    attrMap[ "opaque-paint-event"           ] = Qt::WA_OpaquePaintEvent;
    attrMap[ "paint-on-screen"              ] = Qt::WA_PaintOnScreen;
    attrMap[ "translucent-background"       ] = Qt::WA_TranslucentBackground;  
    return attrMap;
}
}

Qt::WidgetAttribute WindowAttribute( const QString& attr ) {

    const static QMap< QString, Qt::WidgetAttribute > attrMap = AttrMap();
    
    if( !attrMap.contains( attr ) ) {
        throw std::logic_error( "Invalid window attribute " + attr.toStdString() );
        return Qt::WidgetAttribute( 0 );
    }
    else return attrMap[ attr ];
}

}




