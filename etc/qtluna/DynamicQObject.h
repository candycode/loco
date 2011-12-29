#pragma once

#include <QHash>
#include <QList>
#include <QMetaObject>
#include <QObject>

class DynamicSlot {
public:
    virtual void Invoke( void **args ) = 0;
};

class DynamicQObject: public QObject {
public:
    DynamicQObject( QObject *parent = 0 ) : QObject( parent ) { }
    int qt_metacall( QMetaObject::Call c, int id, void **arguments ); 
    bool Connect( QObject *obj, const char *signal, const char *slot );
    virtual DynamicSlot* createSlot( const char *slot ) = 0;
	virtual ~DynamicQObject() {
		for( QList< DynamicSlot* >::iterator i = slotList.begin();
			 i != slotList.end(); ++i ) {
            delete *i;
		}
	}
private:
    QHash< QByteArray, int > slotIndices;
    QList< DynamicSlot* > slotList;
};
