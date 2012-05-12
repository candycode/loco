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


#include <QUrl>
#include <QVariant>
#include <QVariantMap>
#include <QString>
#include <QNetworkRequest>
#include <QDir>

namespace loco {
void AddToUrlQuery( QUrl& url, const QVariantMap& q ) {
    for( QVariantMap::const_iterator i = q.begin(); i != q.end(); ++i ) {
        if( i.value().type() != QVariant::List ) {
            url.addQueryItem( i.key(), i.value().toString() );
        } else {
            QVariantList m = i.value().toList();
            for( QVariantList::const_iterator k = m.begin(); k != m.end(); ++k ) {
                url.addQueryItem( i.key(), ( *k ).toString() );
            }
        }
    }
}
QUrl ConfigureURL( const QString& urlString, const QVariantMap& opt ) {
    QUrl url( urlString );
    if( opt.contains( "query_delimiters" ) ) {
        url.setQueryDelimiters( opt[ "query_delimiters" ].toList().at( 0 ).toChar().toAscii(),
                                opt[ "query_delimiters" ].toList().at( 1 ).toChar().toAscii() );
    }
    if( opt.contains( "username" ) ) url.setUserName( opt[ "username" ].toString() );
    if( opt.contains( "password" ) ) url.setPassword( opt[ "password" ].toString() );
    if( opt.contains( "port" ) ) url.setPort( opt[ "port" ].toInt() );
    if( !opt.contains( "query" ) ) return url;
    QVariantMap q = opt[ "query" ].toMap();
    AddToUrlQuery( url, q );
    return url;
}

void ConfigureHeaders( QNetworkRequest& req, const QVariantMap& headers ) {
    for( QVariantMap::const_iterator i = headers.begin(); i != headers.end(); ++i ) {
        req.setRawHeader( i.key().toAscii(), i.value().toByteArray() );
    }
}

QUrl TranslateUrl( const QString& urlString ) {
    if( urlString.contains( "://" ) ) return QUrl( urlString );
    else if( urlString.startsWith( '/' ) ) return QUrl( "file://" + urlString );
    else {
#ifdef Q_OS_WIN
        return QUrl( "file:///" + QDir::currentPath() + "/" + urlString );
#else
        return QUrl( "file://" + QDir::currentPath() + "/" + urlString );
#endif
    }
}

}
