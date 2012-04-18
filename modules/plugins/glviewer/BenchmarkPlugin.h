#include <QObject>
#include <QtPlugin>
#include <QList>
#include <QVariantList>


struct IDummy {};
Q_DECLARE_INTERFACE(IDummy,"dummy")

class BenchmarkPlugin : public QObject, public IDummy {
	Q_OBJECT
	Q_INTERFACES( IDummy )
public:
signals:
    void emittedSignal( const QVariantList& ) const;
public slots:
    QList< int > copyIntArray( const QList< int >& ia ) const { return ia; }
    QVariant copy( const QVariantList& vl ) const { return vl; }
    void emitSignal( int times, int arrayLength ) const {
    	QVariantList array;
        array.reserve( arrayLength );
        for( int i = 0; i != arrayLength; ++i ) array.push_back( double( i ) );
        for( ; times; --times ) emit emittedSignal( array );
    }
};

Q_EXPORT_PLUGIN2( locoviewer, BenchmarkPlugin )
