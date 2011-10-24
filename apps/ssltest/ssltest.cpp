#include <QtGui/QApplication>
#include <QtCore/QDebug>
#include <QtCore/QList>
#include <QNetworkAccessManager>

#include "handler.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Handler handler;

    // CA certs for:
    // 1. cert with Subject.CN == mail.google.com cert with serial 1f:19:f6:de:35:dd:63:a1:42:91:8a:d5:2c:c0:ab:12
    // 2. cert with Subject.CN == www.google.com cert with serial 2F:DF:BC:F6:AE:91:52:6D:0F:9A:A3:DF:40:34:3E:9A
    /*QList<QSslCertificate> CAcerts =
            // serial 30:00:00:02
            QSslCertificate::fromPath("ThawteSGCCA.crt") +
            // serial 70:BA:E4:1D:10:D9:29:34:B6:38:CA:7B:03:CC:BA:BF
            QSslCertificate::fromPath("BuiltinObjectToken-VerisignClass3PublicPrimaryCertificationAuthority.crt");

    qDebug() << "root CA certificates:\n"
             << CAcerts
             << "\n";
    QSslSocket::setDefaultCaCertificates(CAcerts); // line A*/

    QWebPage page;
    // OK because cert with serial 1f:19:f6:de:35:dd:63:a1:42:91:8a:d5:2c:c0:ab:12 is for host mail.google.com
//  page.mainFrame()->load(QUrl("https://mail.google.com")); // line B1
    // SSL ERROR "The host name did not match any of the valid hosts for this certificate"
    // because cert with serial 1f:19:f6:de:35:dd:63:a1:42:91:8a:d5:2c:c0:ab:12 is NOT for www.gmail.com
    page.mainFrame()->load(QUrl("https://www.gmail.com")); // line B2

    QObject::connect(page.networkAccessManager(), SIGNAL(finished(QNetworkReply*)), &handler, SLOT(slotFinished(QNetworkReply*)));
    QObject::connect(page.networkAccessManager(), SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), &handler, SLOT(slotSslErrors(QNetworkReply*,QList<QSslError>)));
    QObject::connect(&page, SIGNAL(loadFinished(bool)), &handler, SLOT(slotLoadFinished(bool)));

    return app.exec();
}
