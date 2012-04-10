#pragma once
//#SRCHEADER
#include <QObject>
#include <QList>
#include <QPair>
#include <QRegExp>
#include <QString>
#include <QtWebKit/QWebPage>

namespace loco {

typedef QList< QPair< QRegExp, QString > > UrlAgentMap;

class WebPage : public QWebPage {
    Q_OBJECT
public:
    WebPage( QWidget* parent = 0 ) : QWebPage( parent ),
                                     allowInterrupt_( true ),
                                     emitWebActionSignal_( false ) {}
    void SetUserAgentForUrl( const QRegExp& url, const QString& userAgent ) {
        urlAgents_.push_back( qMakePair( url, userAgent ) );
    }
    void SetAllowInterruptJavaScript( bool yes ) {
        allowInterrupt_ = yes;
    }
    bool GetAllowInterruptJavaScript() const {
        return allowInterrupt_;
    }
    void TriggerAction( WebAction action, bool checked ) {
        triggerAction( action, checked );
    }
    void SetEmitWebActionSignal( bool yes ) { emitWebActionSignal_ = yes; }
protected:
    QString userAgentForUrl( const QUrl& url ) const {
        const QString s = url.toString();
        for( UrlAgentMap::const_iterator i = urlAgents_.begin();
             i != urlAgents_.end(); ++i ) {
            if( i->first.exactMatch( s ) ) return i->second;
        }
        return QWebPage::userAgentForUrl( url );
    }
    void triggerAction( WebAction action, bool checked = false ) {
        QWebPage::triggerAction( action, checked );
        if( emitWebActionSignal_ ) emit ActionTriggered( action, checked );
    }
    void javaScriptConsoleMessage(const QString& message, int lineNumber, const QString& sourceID ) {
        QWebPage::javaScriptConsoleMessage( message, lineNumber, sourceID );
        emit JavaScriptConsoleMessage( message, lineNumber, sourceID );
    }
signals:
    void ActionTriggered( QWebPage::WebAction, bool );
    void JavaScriptConsoleMessage( const QString& message, int lineNumber, const QString& sourceID );
public slots:
    bool shouldInterruptJavaScript() {
        if( !allowInterrupt_ ) return false;
        return QWebPage::shouldInterruptJavaScript();
    }
private:
    bool allowInterrupt_;
    UrlAgentMap urlAgents_;
    bool emitWebActionSignal_;
};
}
