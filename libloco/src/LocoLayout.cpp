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

#include "../include/LocoLayout.h"
#include "../include/LocoWidget.h"
#include "../include/LocoWrappedWidget.h"

namespace loco {
void VBoxLayout::addWidget( QObject* w ) { 
    if( qobject_cast< Widget* >( w ) ) {
    Widget* lw = qobject_cast< Widget* >( w );
    layout_->addWidget( qobject_cast< QWidget* >( lw->GetQWidget() ) );
    } else  if( !qobject_cast< QWidget* >( w ) ) {
    error( "VBoxLayout::addWidget() requires a QWidget" );
    } else {
    layout_->addWidget( qobject_cast< QWidget* >( w ) );
    } 
}
void HBoxLayout::addWidget( QObject* w ) { 
    if( qobject_cast< Widget* >( w ) ) {
    Widget* lw = qobject_cast< Widget* >( w );
    layout_->addWidget( qobject_cast< QWidget* >( lw->GetQWidget() ) );
    } else if( qobject_cast< WrappedWidget* >( w ) ) {
        layout_->addWidget( qobject_cast< QWidget* >( qobject_cast< WrappedWidget* >( w )->GetWidget() ) );  
    } else  if( qobject_cast< QWidget* >( w ) ) {
        layout_->addWidget( qobject_cast< QWidget* >( w ) );
    } else {
    error( "VBoxLayout::addWidget() requires a QWidget" );
    } 
}
}
