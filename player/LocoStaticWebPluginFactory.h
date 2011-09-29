#pragma once
//#SRCHEADER

#include <stdexcept>

#include <QString>
#include <QMap>
#include <QVariantMap>
#include <QStringList>
#include <QWebPluginFactory>
#include <QUrl>

#include "LocoObject.h"

namespace loco {

class StaticWebPluginFactory : public QWebPluginFactory {
    typedef QMap< QString, QObject* > ObjectStore;
public:
	StaticWebPluginFactory( QObject* parent = 0 ) : 
        QWebPluginFactory( parent ),
        mimeType_( "application/x-qt-plugin" ) {}
	virtual QObject* create ( const QString& mimeType, 
				  			  const QUrl& url, 
							  const QStringList& argumentNames, 
							  const QStringList& argumentValues ) const {
        if( !objects_.contains( url.toString() ) {
            throw std::runtime_error( url.toString().toStdString() + " not found" );
            return 0;
        }
        
        QObject* webObj = objects_[ url.toString() ];
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
        }  
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
    void InsertObject( const QString& key, QObject* value ) {
        objects_[ key ] = value;    
    }
    ~StaticWebPluginFactory() {
        for( ObjectStore::iterator i = objects_.begin();
             i != objects_.end(); ++i ) {
            if( i->parent() == 0 ) i.value()->deleteLater();
        }
    }
private: 
    QString mimeType_;
    ObjectStore objects_;
};