//#SRCHEADER

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
	else return QUrl( "file://" + QDir::currentPath() + "/" + urlString );
}

}
