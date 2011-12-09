
#include <iostream>
#include <string>

#include <QObject>
#include <QtPlugin>


struct IDummy {};
Q_DECLARE_INTERFACE(IDummy,"dummy")

class SourceObjPlugin : public QObject, public IDummy {
    Q_OBJECT
    Q_INTERFACES( IDummy )
public:
    SourceObjPlugin( QObject* parent = 0 ) : QObject( parent ) {
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
    void link( const QString& signal, QObject* receiver, const QString& slot ) {
        QString mangledSignal = SIGNAL( _XYZ_() );
        mangledSignal = mangledSignal.replace( "_XYZ_()", signal.toAscii().constData() );
        QString mangledSlot = SLOT( _XYZ_() );
        mangledSlot = mangledSlot.replace( "_XYZ_()", slot.toAscii().constData() ); 
        connect( this, mangledSignal.toAscii().constData(), receiver, mangledSlot.toAscii().constData() );
    }
};

Q_EXPORT_PLUGIN2( SourceObjPlugin, SourceObjPlugin )
