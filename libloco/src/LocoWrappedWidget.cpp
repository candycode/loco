//#SRCHEADER
#include "LocoWrappedWidget.h"

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
    Qt::WindowFlags w = 0;
    for( QStringList::const_iterator i = flags.begin(); i != flags.end(); ++i ) {
        const QString& f = *i;
        if( f == "dialog" ) w |= Qt::Dialog;
        else if( f == "window" ) w |= Qt::Window;
        else if( f == "sheet" ) w |= Qt::Sheet;
        else if( f == "drawer" ) w |= Qt::Drawer;
        else if( f == "popup" ) w |= Qt::Popup;
        else if( f == "tool" ) w |= Qt::Tool;
        else if( f == "splash" ) w |= Qt::SplashScreen;
        else if( f == "sub" ) w |= Qt::SubWindow;
        else if( f == "frameless-hint" ) w |= Qt::FramelessWindowHint;
        else if( f == "modal-hint" ) Widget()->setWindowModality( Qt::ApplicationModal );
        else if( f == "sys-menu-hint" ) w |= Qt::WindowSystemMenuHint;
        else if( f == "close-button-hint" ) w |= Qt::WindowCloseButtonHint;
        else if( f == "bypass-graphics-proxy-hint" ) w |= Qt::BypassGraphicsProxyWidget;
        else if( f == "stay-on-top-hint" ) w |= Qt ::WindowStaysOnTopHint;
        else if( f == "stay-on-bottom-hint") w |= Qt ::WindowStaysOnBottomHint;
        else if( f == "ok-button-hint" ) w |= Qt::WindowOkButtonHint;
        else if( f == "cancel-button-hint" ) w |= Qt::WindowCancelButtonHint;
    }
    Widget()->setWindowFlags( w );
}



}

