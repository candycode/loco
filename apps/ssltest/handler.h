#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QSslSocket>
#include <QtNetwork/QSslError>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebPage>



class Handler : public QObject{
    Q_OBJECT

public slots:

    void slotLoadFinished(bool ok) {
        if (ok) {
            qDebug() << "Page size: " << static_cast<QWebPage*>(sender())->mainFrame()->toHtml().size();
        }
    }

    void slotFinished(QNetworkReply * reply) {
        if (reply->error() == QNetworkReply::NoError) {
            qDebug() << "connected to " << reply->url();
            qDebug() << "HTTP status: " << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        } else {
            qDebug() << "error while connecting to " << reply->url();
            qDebug() << "error code: " << reply->error();
            qDebug() << "error string: " << reply->errorString();
        }
    }

    void slotSslErrors(QNetworkReply * reply, QList<QSslError> const & errors) {
        qDebug() << "SSL errors: " << errors;
        qDebug() << "peer's certificate: "
                 << reply->sslConfiguration().peerCertificate();
    }

};
