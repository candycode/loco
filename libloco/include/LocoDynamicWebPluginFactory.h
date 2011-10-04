#pragma once
//#SRCHEADER

#include <stdexcept>

#include <QString>
#include <QList>
#include <QVariantMap>
#include <QStringList>
#include <QWebPluginFactory>
#include <QPluginLoader>
#include <QUrl>
#include <QMetaObject>
#include <QMetaMethod>

#include "LocoObject.h"

namespace loco {

typedef QList< QPluginLoader* > PluginLoaders;

class DynamicWebPluginFactory : public QWebPluginFactory {
    Q_OBJECT
public:
	DynamicWebPluginFactory( QObject* parent = 0 ) : 
        QWebPluginFactory( parent ),
        mimeType_( "application/x-qt-plugin" ),
        initMethodSignature_( "init(QStringList,QStringList)" ) {}
	virtual QObject* create ( const QString& mimeType, 
				  			  const QUrl& url, 
							  const QStringList& argumentNames, 
							  const QStringList& argumentValues ) const {

        //url to filename OR url as filename
        //create plugin loader and set it as child of this
		const QString path = url.toString().remove( 0, QString( "file://" ).size() );
        QPluginLoader* pluginLoader = new QPluginLoader( path );
        QObject* webObj = pluginLoader->instance();
        if( webObj == 0 ) {
            const QString errorString = pluginLoader->errorString();
            delete pluginLoader;
            throw std::runtime_error( "Cannot load web plugin " +
            		                  path.toStdString() + "\n" +
            		                  errorString.toStdString() );
            return 0;
        }
        connect( this, SIGNAL( destroyed( QObject* ) ), this, SLOT( OnDestroy( QObject* ) ) );
        if( qobject_cast< Object* >( webObj ) != 0 ) {
            QVariantMap m;
            QStringList::const_iterator a = argumentNames.begin();
            QStringList::const_iterator v = argumentValues.begin();  
            for( ; a != argumentNames.end() && v != argumentValues.end(); ++a, ++v ) {
                const QString& key = *a;
                const QString& val = *v;
                bool ok = false;
                val.toInt( &ok );
                if( ok ) { m[ key ] = val.toInt(); continue; }
                val.toDouble( &ok );
                if( ok ) { m[ key ] = val.toDouble(); continue; }
                if( val.toLower() == "true" ||
                    val.toLower() == "on"   || 
                    val.toLower() == "yes" ) { m[ key ] = true; continue; }
                if( val.toLower() == "false" ||
                    val.toLower() == "off"   || 
                    val.toLower() == "no" ) { m[ key ] = false; continue; }
                m[ key ] = val;
           }
           qobject_cast< Object* >( webObj )->Init( m );    
        } else {
            const QMetaObject* mo = webObj->metaObject();
            if( mo->indexOfMethod( initMethodSignature_.toAscii().constData() ) >= 0 ) {
                QMetaMethod method = mo->method( mo->indexOfMethod( initMethodSignature_.toAscii().constData() ) );
            	method.invoke( webObj, Qt::DirectConnection,
            			       Q_ARG( QStringList, argumentNames ),
            			       Q_ARG( QStringList, argumentValues) );

            }
        }
        pluginLoaders_.push_back( pluginLoader );
        return webObj;
	}

	virtual QList<Plugin> plugins () const {
		QWebPluginFactory::MimeType mt;
		mt.description = "LoCO plugin";
		mt.fileExtensions << "";
		mt.name = mimeType_;
		QList<QWebPluginFactory::MimeType> mtl;
		mtl.push_back( mt );
		QWebPluginFactory::Plugin p;
		p.description = "LoCO plugin";
		p.mimeTypes = mtl;
		p.name = "LoCO plugin";
		QList< QWebPluginFactory::Plugin > pl;
		pl.push_back( p );
		return pl;
	}
	virtual void refreshPlugins () {}

private slots:

    void OnDestroy( QObject* obj ) {
        for( PluginLoaders::iterator i = pluginLoaders_.begin();
             i != pluginLoaders_.end(); ++i ) {
            ( *i )->unload();
            ( *i )->deleteLater();
        }
    }
private: 
    mutable PluginLoaders pluginLoaders_;
    QString mimeType_;
    QString initMethodSignature_;

};

}
