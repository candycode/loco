#pragma once
//#SRCHEADER

#include <QString>
#include <QVariant>
#include <QVariantMap>
#include <QPointer>

#include "LocoObject.h"
#include "LocoNetworkAccessManager.h"

namespace loco {

class Http : public Object {
	Q_OBJECT
private:
	typedef QVariantMap (Http::*Request)(const QString& , const QVariant& , int );
public:
	Http( NetworkAccessManager* nam ) :
		Object( 0, "LocoHttp", "/Loco/Network/Http" ), nam_( nam ) {}
public slots:
    QVariantMap get ( const QString& urlString, int timeout, const QVariantMap& opt = QVariantMap() );
    QVariantMap post( const QString& urlString,
    		          int timeout,
    		          const QByteArray& data,
    		          const QVariantMap& opt = QVariantMap() );
    QVariantMap head( const QString& urlString, int timeout, const QVariantMap& opt = QVariantMap() );
    QVariantMap request( const QString& urlString,
	                     const QByteArray& verb,
    		             int timeout,
	                     const QVariantMap& opt = QVariantMap(),
	                     const QByteArray& data = QByteArray() );
private:
    QPointer< NetworkAccessManager > nam_;
};

class Network : public Object {
    Q_OBJECT
public:
    Network() : Object( 0, "LocoNetwork", "/Loco/Network" ) {}
public slots:
    QVariant create( const QString& );
};

}
