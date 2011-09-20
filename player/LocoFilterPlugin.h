#pragma once
//#SRCHEADER

#include "EWL.h"
#include "LocoFilter.h"


namespace loco {
class FilterPlugin : public EWL  {
	Q_OBJECT
	Q_INTERFACES( Filter )
public:
	FilterPlugin(); //implement in the cpp file creating the
	                 //desired concrete instance of Factory
    QString Filter( const QString& code ) {
    	return filter_->Filter( code );
    }
    void destroy( Filter* obj ) { obj->deleteLater(); }
private:
    Filter* filter_;     	
};
}

Q_DECLARE_INTERFACE( loco::Filter, "Loco/Filter:1.0" )