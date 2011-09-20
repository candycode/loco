#pragma once
//#SRCHEADER
#include <QVariant>
#include <QDesktopWidget>
#include <QRect>
#include "LocoObject.h"

namespace loco {

class GUI : public Object {
    Q_OBJECT

public:
   GUI() : Object( 0, "LocoGUI", "Loco/GUI" ) {}

public slots:
    
    QVariant create( const QString& name, const QVariantMap& params = QVariantMap() ) const;

	 QVariantMap desktopGeometry( int screen = -1 ) {
        const QDesktopWidget dw;
        const QRect geom = dw.availableGeometry();
        QVariantMap vm;
        vm[ "screen" ] = screen;
        vm[ "x" ] = geom.x();
        vm[ "y" ] = geom.y();
        vm[ "w" ] = geom.width();
        vm[ "h" ] = geom.height();
        vm[ "screens" ] = dw.screenCount();
        return vm;
    }       
    
//    QVariantMap colorDialog( const QVariantMap& c = QVariantMap() );
   

};

}