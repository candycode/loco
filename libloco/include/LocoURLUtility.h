#pragma once
//#SRCHEADER

#include <QVariantMap>

class QUrl;
class QVariant;
class QNetworkRequest;
class QString;

namespace loco {

void AddToUrlQuery( QUrl& url, const QVariantMap& q );
QUrl ConfigureURL( const QString& urlString, const QVariantMap& opt );
void ConfigureHeaders( QNetworkRequest& req, const QVariantMap& headers );
QUrl TranslateUrl( const QString& urlString );

}
