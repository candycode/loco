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


#include <QtWebKit/QWebView>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtWebKit/QWebFrame>
#include <QTimer>
#include <QEventLoop>
#include <QVariantMap>
#include <QList>
#include <QPair>
#include <QtWebKit/QWebPage>
#include <QRegExp>
#include <QtWebKit/QWebElement>
#include <QtWebKit/QWebElementCollection>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDir>

#include "LocoWebPage.h"
#include "LocoURLUtility.h"


class QWebPluginFactory;

namespace loco {

class WebView : public QWebView {
    Q_OBJECT
public:
    WebView(); 
    void SetUserAgentForUrl( const QRegExp& url, const QString& userAgent ) {
        qobject_cast< WebPage* >( page() )->SetUserAgentForUrl( url, userAgent );
    }
    void SetAllowInterruptJavaScript( bool yes ) {
        qobject_cast< WebPage* >( page() )->SetAllowInterruptJavaScript( yes );
    }
    void EatContextMenuEvent( bool yes ) { eatContextMenuEvent_ = yes; }
    bool EatingContextMenuEvent() const { return eatContextMenuEvent_; }
    void EatKeyEvents( bool yes ) { eatKeyEvents_ = yes; }
    bool EatingKeyEvents() const { return eatKeyEvents_; }
    void EatMouseEvents( bool yes ) { eatMouseEvents_ = yes; }
    bool EatingMouseEvents() const { return eatMouseEvents_; }
    void SetWebPluginFactory( QWebPluginFactory* pf ) {
        page()->setPluginFactory( pf );
    }
    QWebPluginFactory* GetWebPluginFactory() const { return page()->pluginFactory(); } 
    void SetPageSize( int w, int h ) { page()->setViewportSize( QSize( w, h ) ); }
    QSize PageSize() const { return page()->viewportSize(); }
    void SaveSnapshot( const QString& filePath, int quality = -1 ) const;
    QPixmap Snapshot() const {
        QPixmap p( page()->viewportSize() );
        QPainter painter( &p );
        page()->mainFrame()->render( &painter, QWebFrame::ContentsLayer );
        return p;
    }
    QString SavePDF( const QString& filePath, const QVariantMap& options = QVariantMap() ) const;
    bool SyncLoad( const QUrl& url, int timeout );
    bool SyncLoad( const QString& url, int timeout ) {
        return SyncLoad( QUrl( TranslateUrl( url ) ), timeout );
    }
    bool SyncLoad( const QString& urlString, const QVariantMap& opt, int timeout ) {
        return SyncLoad( ConfigureURL( urlString, opt ), timeout );
    }
    void Load( const QString& urlString, const QVariantMap& opt ) {
        load( ConfigureURL( urlString, opt ) );
    }
    void Load( const QString& urlString ) {
        load( TranslateUrl( urlString ) );
    }
    QWebElement FindFirstElement( const QString& selector )  {
        return page()->mainFrame()->findFirstElement( selector );
    }
    QWebElementCollection FindElements( const QString& selector ) {
        return page()->mainFrame()->findAllElements( selector );
    }
    void HighlightText( const QString& substring ) {
        page()->findText( substring, QWebPage::HighlightAllOccurrences );
    }
    bool SaveUrl( const QString& url, const QString& filename, int timeout );
    bool TriggerAction( const QString&, bool );
    void SetEmitWebActionSignal( bool yes ) {
        WebPage* wp = qobject_cast< WebPage* >( page() );
        if( !wp ) return;
        wp->SetEmitWebActionSignal( yes );
    }
private slots:
    void OnLoadFinished( bool ok ) { syncLoadOK_ = ok; }
    void OnDownloadRequested( const QNetworkRequest& nr ) {
        emit downloadRequested( nr.url().toString() );
    }
    void OnUnsupportedContent( QNetworkReply* nr ) {
        emit unsupportedContent( nr->url().toString() );
    }
    void OnFrameCreated( QWebFrame* wf ) {
        emit onFrameCreated( wf );
    }
    void OnActionTriggered( QWebPage::WebAction, bool );

protected:
    void closeEvent( QCloseEvent* e ) {
        QWebView::closeEvent( e );
        emit closing();
    }
    void contextMenuEvent( QContextMenuEvent* e ) {
        if( !eatContextMenuEvent_ ) QWebView::contextMenuEvent( e );
    }
    void keyPressEvent( QKeyEvent* ke ) {
        if( !eatKeyEvents_ ) QWebView::keyPressEvent( ke );
        emit keyPress( ke->key(), ke->modifiers(), ke->count() );

    }
    void keyReleaseEvent( QKeyEvent* ke ) {
        if( !eatKeyEvents_ ) QWebView::keyReleaseEvent( ke );
        emit keyPress( ke->key(), ke->modifiers(), ke->count() );

    }
    void mouseMoveEvent( QMouseEvent* me ) {
        if( !eatMouseEvents_ ) QWebView::mouseMoveEvent( me );
        emit mouseMove( me->globalX(),
                        me->globalY(),
                        me->x(),
                        me->y(),
                        bool( me->buttons() & Qt::LeftButton ),
                        bool( me->buttons() & Qt::MiddleButton ),
                        bool( me->buttons() & Qt::RightButton ) );

    }
    void mousePressEvent( QMouseEvent* me ) {
        if( !eatMouseEvents_ ) QWebView::mousePressEvent( me );
        emit mousePress( me->globalX(),
                         me->globalY(),
                         me->x(),
                         me->y(),
                         bool( me->buttons() & Qt::LeftButton ),
                         bool( me->buttons() & Qt::MiddleButton ),
                         bool( me->buttons() & Qt::RightButton ) );

    }
    void mouseReleaseEvent( QMouseEvent* me ) {
        if( !eatMouseEvents_ ) QWebView::mouseReleaseEvent( me );
        emit mouseRelease( me->globalX(),
                           me->globalY(),
                           me->x(),
                           me->y(),
                           bool( me->buttons() & Qt::LeftButton ),
                           bool( me->buttons() & Qt::MiddleButton ),
                           bool( me->buttons() & Qt::RightButton ) );

    }
    void mouseDoubleClickEvent( QMouseEvent* me ) {
        if( !eatMouseEvents_ ) QWebView::mouseDoubleClickEvent( me );
        emit mouseDoubleClick( me->globalX(),
                               me->globalY(),
                               me->x(),
                               me->y(),
                               bool( me->buttons() & Qt::LeftButton ),
                               bool( me->buttons() & Qt::MiddleButton ),
                               bool( me->buttons() & Qt::RightButton ) );

    }
signals:
    void keyPress( int, int, int );
    void keyRelease( int, int, int );
    void mousePress( int, int, int, int, bool, bool, bool );
    void mouseRelease( int, int, int, int, bool, bool, bool );
    void mouseMove( int, int, int, int, bool, bool, bool );
    void mouseDoubleClick( int, int, int, int, bool, bool, bool );
    void closing();
    void downloadRequested( const QString& );
    void unsupportedContent( const QString& );
    void actionTriggered( const QString&, bool );
    void fileDownloadProgress( qint64, qint64 );
    void onFrameCreated( QWebFrame* );
    void JavaScriptConsoleMessage( const QString&, int, const QString& );
private:
    bool eatContextMenuEvent_;
    bool eatKeyEvents_;
    bool eatMouseEvents_;
    mutable bool syncLoadOK_;
};
}
