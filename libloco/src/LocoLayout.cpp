
#include "../include/LocoLayout.h"
#include "../include/LocoWidget.h"

namespace loco {
void Layout::addWidget( QObject* w ) { 
    if( qobject_cast< Widget* >( w ) ) {
	Widget* lw = qobject_cast< Widget* >( w );
	layout_->addWidget( qobject_cast< QWidget* >( lw->GetQWidget() ) );
    } else  if( !qobject_cast< QWidget* >( w ) ) {
	error( "VBoxLayout::addWidget() requires a QWidget" );
    } else {
	layout_->addWidget( qobject_cast< QWidget* >( w ) );
    } 
}
}
