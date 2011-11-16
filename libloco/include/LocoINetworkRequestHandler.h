#pragma once
//#SRCHEADER

#include <QNetworkAccessManager>

class QNetworkRequest;
class QIODevice;

namespace loco {
struct INetworkRequestHandler {
	virtual QNetworkReply* HandleRequest( QNetworkAccessManager::Operation op,
                                          const QNetworkRequest& req,
                                          QIODevice* outgoingData ) = 0;
	virtual ~INetworkRequestHandler() {}
};
}
