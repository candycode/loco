#pragma once
////////////////////////////////////////////////////////////////////////////////
//Copyright (c) 2012, Ugo Varetto
//All rights reserved.
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions are met:
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder nor the
//      names of its contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//DISCLAIMED. IN NO EVENT SHALL UGO VARETTO BE LIABLE FOR ANY
//DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////


#include <stdexcept>

#include <QString>
#include <QList>
#include <QVariantMap>
#include <QStringList>
#include <QtWebKit/QWebPluginFactory>
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

        QString path = url.toLocalFile();
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
