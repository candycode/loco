
#include <iostream>
#include <string>

#include <QObject>
#include <QtPlugin>

class TargetObject : public QObject {
    Q_OBJECT
public:
    TargetObject( QObject* parent = 0 ) : QObject( parent ) {
        setObjectName( "TargetObject" );
    }
public slots:
    // cannot call following method from Javascript because std::string
    // not marshalled between js <--> c++
    void aSlot( const std::string& msg ) {
        std::cout << "TargetObject::aSlot: " 
                  << msg << " received from " 
                  << sender()->objectName().toStdString() << std::endl;
    }
};

class SourceObject : public QObject {
    Q_OBJECT
public:
    SourceObject( QObject* parent = 0 ) : QObject( parent ) {
        setObjectName( "SourceObject" );
    }
signals:
    // cannot call following signal directly from js because
    // std string not marshalled between js <--> c++
    void aSignal( const std::string& msg );
public slots:
    //call from javascript
    void emitSignal( const QString& msg ) {
        emit aSignal( msg.toStdString() );
    }
};


struct IDummy {};
Q_DECLARE_INTERFACE(IDummy,"dummy")

class SigSlotTestPlugin : public QObject, public IDummy {
	Q_OBJECT
	Q_INTERFACES( IDummy )
public slots:
    void setContext(  
    QVariant createSource() const {
    }
    QVariant createTarget() const {
    }
};

Q_EXPORT_PLUGIN2( <plugin name>, Plugin )
//                   ^              ^
//                 plugin name    name of c++ class implementing the plugin
