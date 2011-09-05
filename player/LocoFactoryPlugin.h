#pragma once
//#SRCHEADER

#include <QObject>

#include "LocoFactory.h"
#include "LocoObject.h"


namespace loco {
class FactoryPlugin : public QObject, Factory {
	Q_OBJECT
	Q_INTERFACES( Factory )
public:
	FactoryPlugin(); //implement in the cpp file creating the
	                 //desired concrete instance of Factory
    Object* create( const QString& uri ) {
    	return factory_->create( uri );
    }
    void destroy( Object* obj ) { obj->deleteLater(); }
private:
    Factory* factory_;     	
};
}

Q_DECLARE_INTERFACE( loco::Factory, "Loco/Factory:1.0" )