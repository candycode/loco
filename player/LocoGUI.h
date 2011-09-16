#pragma once
//#SRCHEADER
#include <QVariant>
#include "LocoObject.h"

namespace loco {

class GUI : public Object {
    Q_OBJECT

public:
   GUI() : Object( 0, "LocoGUI", "Loco/GUI" ) {}

public slots:
    
    QVariant create( const QString& name ) const;
    
//    QVariantMap colorDialog( const QVariantMap& c = QVariantMap() );
   

};

}