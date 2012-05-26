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

#include "LocoWrappedWidget.h"
#include "LocoWindowFlags.h"

namespace loco {
//slots:
void WrappedWidget::setMouseCursor( const QString& shape ) {
    if( shape == "arrow" ) Widget()->setCursor( Qt::ArrowCursor );
    else if( shape == "upArrow" ) Widget()->setCursor( Qt::UpArrowCursor );
    else if( shape == "cross" )  Widget()->setCursor( Qt::CrossCursor );
    else if( shape == "wait" ) Widget()->setCursor( Qt::WaitCursor );
    else if( shape == "ibeam" ) Widget()->setCursor( Qt::IBeamCursor );
    else if( shape == "sizeVert" ) Widget()->setCursor( Qt::SizeVerCursor );
    else if( shape == "sizeHor" ) Widget()->setCursor( Qt::SizeHorCursor );
    else if( shape == "wait" ) Widget()->setCursor( Qt::WaitCursor );
    else if( shape == "sizeAll" ) Widget()->setCursor( Qt::SizeAllCursor );
    else if( shape == "hide" ) Widget()->setCursor( Qt::BlankCursor );
    else if( shape == "splitVert" ) Widget()->setCursor( Qt::SplitVCursor );
    else if( shape == "splitHor" ) Widget()->setCursor( Qt::SplitHCursor );
    else if( shape == "pointHand" ) Widget()->setCursor( Qt::PointingHandCursor );
    else if( shape == "forbid" ) Widget()->setCursor( Qt::ForbiddenCursor );
    else if( shape == "openHand" ) Widget()->setCursor( Qt::OpenHandCursor );
    else if( shape == "closedHand" ) Widget()->setCursor( Qt::ClosedHandCursor );
    else if( shape == "whatsThis" ) Widget()->setCursor( Qt::WhatsThisCursor );
    else if( shape == "busy" ) Widget()->setCursor( Qt::BusyCursor );
    else if( shape == "dragMove" ) Widget()->setCursor( Qt::DragMoveCursor );
    else if( shape == "dragCopy" ) Widget()->setCursor( Qt::DragCopyCursor );
    else if( shape == "dragLink" ) Widget()->setCursor( Qt::DragLinkCursor );

}

void WrappedWidget::setWindowStyle( const QStringList& flags ) {
    Widget()->setWindowFlags( WindowFlags( flags ) );
    if( flags.contains( "modal-hint" ) ) Widget()->setWindowModality( Qt::ApplicationModal );
}



}

