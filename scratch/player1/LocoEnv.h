#pragma once
#ifndef LOCO_ENV_
#define LOCO_ENV_
//#SRCHEADER
#include <QObject>
#include <QVariantMap>
#include <QString>
class LocoEnv : public QObject {
    Q_OBJECT
public slots:
	QVariant loadModule( const QString& path );
	QVariantMap queryModule( const QString& moduleName ) const;
	QVariantMap queryInterface( const QObject& obj ); 
    QStringList listModules() const;
    QVariant create( const QString& moduleName, 
                     const QString& ifaceName,
                     const QVariantMap& parameters );
    QVariant releaseModule( const QVariant& id );                                                              	
};
#endif