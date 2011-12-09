
#include <iostream>
#include <string>

#include <QObject>
#include <QtPlugin>

struct IDummy {};
Q_DECLARE_INTERFACE(IDummy,"dummy")

class TargetObjPlugin : public QObject, public IDummy {
    Q_OBJECT
    Q_INTERFACES( IDummy )
public:
    TargetObjPlugin( QObject* parent = 0 ) : QObject( parent ) {
        setObjectName( "TargetObject" );
    }
public slots:
    // cannot call following method from Javascript because std::string
    // is not marshalled between js <--> c++
    void aSlot( const std::string& msg ) {
        std::cout << "TargetObject::aSlot: \""
        		  << msg <<  "\" received from "
                  << sender()->objectName().toStdString() << std::endl;
    }
};

Q_EXPORT_PLUGIN2( TargetObjPlugin, TargetObjPlugin )
