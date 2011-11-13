#pragma once
//#SRCHEADER

#include "LocoObject.h"

class QWidget;

namespace loco {

//to be able to cast from QObject we need to pass a QObject derived object to qobject_cast 

class  WrappedWidget : public Object {
	Q_OBJECT
public:
	WrappedWidget( Context* c = 0, 
		           const QString& n = "LocoWrappedWidget",
			       const QString& type = "", 
			       const QString& module = "",
                   bool destroyable = false,
				   ObjectInfo* objInfo = 0 ) : Object( c, n, type, module, destroyable, objInfo ) {}
public:
	virtual QWidget* Widget()  = 0;
	virtual const QWidget* Widget() const = 0;
public:
	virtual ~WrappedWidget() {}
};

}