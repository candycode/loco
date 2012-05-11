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
