
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
