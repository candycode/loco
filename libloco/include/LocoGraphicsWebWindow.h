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
#include <QString>
#include <QVariantMap>
#include <QMap>
#include <QtGui/QPixmap>
#include <QtGui/QBitmap>
#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebSettings>
#include <QtWebKit/QWebElementCollection>
#include <QtWebKit/QWebElement>
#include <QtWebKit/QWebInspector>
#include <QtGui/QCursor>
#include <QtWebKit/QGraphicsWebView>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QAction>

#include "LocoObject.h"
#include "LocoContext.h"
#include "LocoWebKitAttributeMap.h"
#include "LocoWebKitJSCoreWrapper.h"
#include "LocoDynamicWebPluginFactory.h"
#include "LocoStaticWebPluginFactory.h"
#include "LocoNetworkAccessManager.h"
#include "LocoWrappedGraphicsWidget.h"

class QWebPluginFactory;

namespace loco {

//temporary QGraphicsView proxy which forwards mouse event
//targeting the page background
class TransparentGraphicsWebView : public QGraphicsWebView {
private:
    bool HandleEvent( QGraphicsSceneMouseEvent* ev ) {
        QWebHitTestResult tr = this->page()->currentFrame()
            ->hitTestContent( ev->scenePos().toPoint() );
        if( tr.element().tagName() == "HTML" ) {
            focus_ = false;
            return false;
        }
        focus_ = true;
        return true;
    }
protected:
    void contextMenuEvent ( QGraphicsSceneContextMenuEvent* ev ) {
        ev->setAccepted( false );
    }
    void keyPressEvent( QKeyEvent* event ) {
        if( focus_ ) {
            QGraphicsWebView::keyPressEvent( event );
        } else event->setAccepted( false );
    }
    void keyReleaseEvent( QKeyEvent* event ) {
        if( focus_ ) {
            QGraphicsWebView::keyReleaseEvent( event );
        } else event->setAccepted( false );
    }
    void mousePressEvent( QGraphicsSceneMouseEvent * ev ) {
        if( HandleEvent( ev ) ) QGraphicsWebView::mousePressEvent( ev );
        else ev->setAccepted( false );
    }
    void mouseReleaseEvent( QGraphicsSceneMouseEvent * ev ) {
        if( HandleEvent( ev ) ) {
            QGraphicsWebView::mouseReleaseEvent( ev );
        }
        else ev->setAccepted( false );
    }
    void mouseMoveEvent( QGraphicsSceneMouseEvent * ev ) {
        QPointF p = mapFromScene( ev->scenePos().x(), ev->scenePos().y() );
        if( focus_ ) QGraphicsWebView::mouseMoveEvent( ev );
        else ev->setAccepted( false );
    }
    void mouseDoubleClickEvent( QGraphicsSceneMouseEvent * ev ) {
        if( HandleEvent( ev ) ) QGraphicsWebView::mouseDoubleClickEvent( ev );
        else ev->setAccepted( false );
    }
    void wheelEvent( QGraphicsSceneWheelEvent* ev ) {
        //if( HandleEvent( ev ) ) QGraphicsWebView::wheelEvent( ev );
        ev->setAccepted( false );
    }
public:
   TransparentGraphicsWebView() : focus_( true ) {}
private:
   bool focus_;

};


class GraphicsWebWindow : public WrappedGraphicsWidget {
    Q_OBJECT
    Q_PROPERTY( bool resizesToContents READ GetResizesToContents WRITE SetResizesToContents )
private:
    struct ObjectEntry {
        QObject* obj_;
        QString jsName_;
        bool own_;
        ObjectEntry( QObject* obj, const QString& jsName, bool own ) :
            obj_( obj ), jsName_( jsName ), own_( own ) {}
    };
    typedef QList< ObjectEntry > ContextObjects;
    bool SyncLoad( const QUrl&, int /*ms*/ );
public:
    GraphicsWebWindow() : WrappedGraphicsWidget( 0, "LocoGraphicsWebWindow", "Loco/GUI/GraphicsWindow" ),
        webView_( new TransparentGraphicsWebView() ), jsInterpreter_( new WebKitJSCoreWrapper )  {
        wf_ = webView_->page()->mainFrame();
        ws_ = webView_->page()->settings();
        connect( webView_->page(), SIGNAL( loadFinished( bool ) ), this, SIGNAL( loadFinished( bool ) ) );
        connect( webView_->page(), SIGNAL( loadProgress( int ) ), this, SIGNAL( loadProgress( int ) ) );
        connect( webView_->page(), SIGNAL( loadStarted() ), this, SIGNAL( loadStarted() ) );
        connect( webView_->page(), SIGNAL( linkClicked( const QUrl& ) ), this, SIGNAL( linkClicked( const QUrl& ) ) );
        connect( &ctx_, SIGNAL( JSContextCleared() ), this, SLOT( PreLoadCBack() ) );
        connect( jsInterpreter_, SIGNAL( JavaScriptContextCleared() ), this, SLOT( JavaScriptContextCleared() ) );
        jsInterpreter_->setParent( this );
        jsInterpreter_->SetWebPage( webView_->page() );   
        ctx_.Init( jsInterpreter_ );
        ctx_.SetJSContextName( "wctx" ); //web window context
        ctx_.AddContextToJS();
    }
    bool GetResizesToContents() const { return webView_->resizesToContents(); }
    void SetResizesToContents( bool on ) { webView_->setResizesToContents( on ); }
    QGraphicsWidget* Widget() { return webView_; }
    virtual const QGraphicsWidget* Widget() const { return webView_; }
    ~GraphicsWebWindow() { if( webView_ && webView_->parent() == 0 ) webView_->deleteLater(); }

    void AddSelfToJSContext() {
        ctx_.AddJSStdObject( this );
    }
    void SetNetworkAccessManager( QNetworkAccessManager* nam ) {
        NetworkAccessManager* na = qobject_cast< NetworkAccessManager* >( nam );
        if( na != 0 ) {
            connect( na, SIGNAL( OnRequest( const QVariantMap& ) ),
                     this, SIGNAL( onRequest( const QVariantMap& ) ) );
        }
        webView_->page()->setNetworkAccessManager( nam );
    }
    void SetContext( Context* ctx ) {
        Object::SetContext( ctx );
        ctx_.SetParentContext( GetContext() );
    }

    QGraphicsWebView* GetWebView() const { return webView_; }

    void SetWebPluginFactory( QWebPluginFactory* wpf ) { webView_->page()->setPluginFactory( wpf ); }

    QWebPluginFactory* GetWebPluginFactory() const { return webView_->page()->pluginFactory(); }

private slots:
    void JavaScriptContextCleared() {
        for( ContextObjects::const_iterator i = ctxObjects_.begin();
             i != ctxObjects_.end(); ++i ) {
           ctx_.AddQObjectToJSContext( i->obj_, i->jsName_, i->own_ ); 
        }   
    }
    void PreLoadCBack() { ctx_.Eval( preLoadCBack_ ); }
    void OnLoadFinished( bool ok ) { syncLoadOK_ = ok; }
public slots:
    bool syncLoad( const QString& url, int timeout /*seconds*/ = -1 ) {
        return SyncLoad( url, 1000 * timeout );
    }
    void setTransparent() {
        QPalette palette = webView_->palette();
        palette.setBrush(QPalette::Base, Qt::transparent);
        webView_->page()->setPalette(palette);
        webView_->setAttribute(Qt::WA_OpaquePaintEvent, false);
    }
    void openInspector() {
        QWebInspector* wi = new QWebInspector;
        wi->setPage( webView_->page() );
        wi->show();
    }
    void setPreLoadCBack( const QString& cback ) { preLoadCBack_ = cback; }
    void addObjectToContext( QObject* obj, const QString& jsName, bool own = false ) {
        ctx_.AddQObjectToJSContext( obj, jsName, own );
        ctxObjects_.push_back( ObjectEntry( obj, jsName, own ) );
    }
    void addNewObjectToContext( QObject* qobj, const QString& jsName, bool own = false ) {
        // duplicate from JSContext, move into Context
        Object* obj = dynamic_cast< Object* >( qobj );
        if( !obj ) {
            error( "loco::Object instance required" );
            return;
        }
        if( !obj->cloneable() ) {
            error( "Object cannot be copied" );
            return;
        }
        obj = obj->Clone();
        obj->SetJSInstanceName( jsName );
        ctx_.AddQObjectToJSContext( obj, jsName, own );
        ctxObjects_.push_back( ObjectEntry( obj, jsName, own ) );
    }
    void resetObjects() { ctxObjects_.clear(); }
    ///@todo void syncLoad( const QUrl& url, int timeout );
    void setLinkDelegationPolicy( const QString& p ) {
        if( p == "all" ) webView_->page()->setLinkDelegationPolicy( QWebPage::DelegateAllLinks );
        else if( p == "external" ) webView_->page()->setLinkDelegationPolicy( QWebPage::DelegateExternalLinks );
        else if( p == "none ") webView_->page()->setLinkDelegationPolicy( QWebPage::DontDelegateLinks );
        else error( "Wrong link delegation policy '" + p +"'" );
    }
    QString webKitVersion() const { return QTWEBKIT_VERSION_STR; }
    bool canLogRequests() const {
        return qobject_cast< NetworkAccessManager* >(
               webView_->page()->networkAccessManager() ) != 0;
    }
    bool canEmitRequestSignal() const {
        return qobject_cast< NetworkAccessManager* >(
               webView_->page()->networkAccessManager() ) != 0;
    }
    void logRequests( bool yes ) {
        NetworkAccessManager* nam = qobject_cast< NetworkAccessManager* >(
               webView_->page()->networkAccessManager() );
        if( !nam ) return;
        nam->SetLogRequestsEnabled( yes );
    }
    void emitRequestSignal( bool yes ) {
        NetworkAccessManager* nam = qobject_cast< NetworkAccessManager* >(
                   webView_->page()->networkAccessManager() );
        if( !nam ) return;
        nam->EmitRequestSignal( yes );
    }
    void createWebPluginFactory( const QString& type = "dynamic" ) {
        if( type.toLower() == "dynamic" ) {
            DynamicWebPluginFactory* pf = new DynamicWebPluginFactory;
            webView_->page()->setPluginFactory( pf );
        } else {
            error( "only 'dynamic' supported at this time" );
        }
    }
    void enableAction( QWebPage::WebAction action, bool yes ) { webView_->pageAction( action )->setEnabled( yes ); }
    bool isActionEnabled( QWebPage::WebAction action ) const { return webView_->pageAction( action )->isEnabled(); }
    void triggerAction( QWebPage::WebAction action, bool checked = false ) { webView_->triggerPageAction( action, checked ); }
    void setName( const QString& n ) { Object::setName( n ); }
    void addSelfToJSContext() { AddSelfToJSContext(); }
    QString frameName() const { return wf_->frameName(); }
    //connect from parent context Context::onError() -> WebWindow::onParentError()
    void onParentError( const QString& err ) {
        ctx_.Eval( "throw " + err + ";\n" );
    }
    ///@todo make it synchronous  
    void load( QString url ) {
        if( !url.contains( "://" ) ) {
        #ifdef Q_OS_WIN 
            url = "file:///" + QDir().absoluteFilePath( url );
        #else
            url = "file://" + QDir().absoluteFilePath( url );
        #endif
        }
        webView_->load( QUrl( url ) ); 
    }
    bool isModified() { return webView_->isModified(); }
    //QList< QWebHistoryItem > history();
    void setHtml( const QString& html, const QString& baseUrl = "" ) { webView_->setHtml( html, QUrl( baseUrl ) ); }
    void setBaseUrl( const QString& url ) { webView_->page()->mainFrame()->setUrl( url ); }
    void setZoomFactor( qreal zf ) { webView_->setZoomFactor( zf ); }
    qreal zoomFactor() const { return webView_->zoomFactor(); }
    void reload() { webView_->reload(); }
    //QVariant geometry() const;
    //void setGeometry( const QVariant& g );
    void setContentEditable( bool yes ) { webView_->page()->setContentEditable( yes ); }
    qint64 totalBytes() const { return webView_->page()->totalBytes(); }
    void setAttribute( const QString& attr, bool on ) {
        webView_->page()->settings()->setAttribute( attrMap_[ attr ], on );
    }
    bool testAttribute( const QString& attr) const {
        return webView_->page()->settings()->testAttribute( attrMap_[ attr ] );
    }
    void setAttributes( const QVariantMap& attr ) {
        QWebSettings& s = *( webView_->page()->settings() );
        for( QVariantMap::const_iterator i = attr.begin();
            i != attr.end(); ++i ) {
            s.setAttribute( attrMap_[ i.key() ], i.value().toBool() );
        }
    }
    QVariant getAttributes() const {
           const QWebSettings& s = *( webView_->page()->settings() );
           AttributeTextMap::const_iterator i = attrMap_.GetAttributeToTextMap().begin();
           const AttributeTextMap::const_iterator e = attrMap_.GetAttributeToTextMap().end();
           QVariantMap m;
           for( ; i != e; ++i ) {
               m[ i.value() ] = s.testAttribute( i.key() );
           }
           return m;
    }
    void setLocalStoragePath( const QString& sp ) { ws_->setLocalStoragePath( sp ); }     
    QString localStoragePath() const { return ws_->localStoragePath(); }
    void setUserCSS( const QString& url ) { ws_->setUserStyleSheetUrl( QUrl( url ) ); }
    QString userCSS() const { return ws_->userStyleSheetUrl().toString(); }
    void enablePersistentStorage( const QString& p = QString() ) { ws_->enablePersistentStorage( p ); }
    void setMaxPagesInCache( int m ) { ws_->setMaximumPagesInCache( m ); }
    void setOfflineStoragePath( const QString& p ) { ws_->setOfflineStoragePath( p ); }
    void setOfflineStorageQuota( quint64 q ) { ws_->setOfflineStorageDefaultQuota( q ); }
    quint64 offlineStorageQuota() const { return ws_->offlineStorageDefaultQuota(); }
    QString offlineStoragePath() const { return ws_->offlineStoragePath(); }
    void setOfflineWebApplicationCachePath( const QString& cp ) { ws_->setOfflineWebApplicationCachePath( cp ); }
    void setOfflineWebApplicationCacheQuota( qint64 q ) { ws_->setOfflineWebApplicationCacheQuota( q ); }    
    QVariant eval( const QString& code, const QStringList& filters = QStringList() ) {
        return ctx_.Eval( code, filters );
    }  
    void addParentObjectsToJS() { 
        ctx_.SetAddObjectsFromParentContext( true );
    }
    QString toHtml() const { return wf_->toHtml(); }
    QString selectedText() const { return webView_->page()->selectedText(); }
    void configScrolling( const QVariantMap& sc ) {
        const QString& h = sc[ "h" ].toString();
        const QString& v = sc[ "v" ].toString();
        Qt::ScrollBarPolicy sbp;
        if( h == "on" ) sbp = Qt::ScrollBarAlwaysOn;
        else if( h == "off" ) sbp = Qt::ScrollBarAlwaysOff;
        else sbp = Qt::ScrollBarAsNeeded;
        wf_->setScrollBarPolicy( Qt::Horizontal, sbp );
        if( v == "on" ) sbp = Qt::ScrollBarAlwaysOn;
        else if( v == "off" ) sbp = Qt::ScrollBarAlwaysOff;
        else sbp = Qt::ScrollBarAsNeeded;
        wf_->setScrollBarPolicy( Qt::Vertical, sbp );
    }
    QVariantMap scrollingConfig() {
        QVariantMap sc;
        Qt::ScrollBarPolicy sbp = wf_->scrollBarPolicy( Qt::Vertical );
        if( sbp == Qt::ScrollBarAlwaysOn ) sc[ "v" ] = "on";
        else if( sbp == Qt::ScrollBarAlwaysOff ) sc[ "v" ] = "off";
        else sc[ "v" ] = "as needed";
        sbp = wf_->scrollBarPolicy( Qt::Horizontal );
        if( sbp == Qt::ScrollBarAlwaysOn ) sc[ "h" ] = "on";
        else if( sbp == Qt::ScrollBarAlwaysOff ) sc[ "h" ] = "off";
        else sc[ "h" ] = "as needed";
        return sc;
    }
    void scrollToAnchor( const QString& a ) { wf_->scrollToAnchor( a ); }
signals:
    void linkClicked( const QUrl& );
    void linkHovered( const QString&, const QString&, const QString& );
    void loadFinished( bool );
    void loadProgress( int );
    void loadStarted();
    void urlChanged( const QUrl& );
    void onRequest( const QVariantMap& );
private:
    QGraphicsWebView* webView_; //owned by this object
    Context ctx_; // this is where objects are created
    QPointer< QWebFrame > wf_; //owned by webview
    QWebSettings* ws_; //owned by web frame; not a QObject: cannot wrap with a QPointer
    WebKitAttributeMap attrMap_;
    WebKitJSCoreWrapper* jsInterpreter_;
    QString preLoadCBack_;
    ContextObjects ctxObjects_;
    bool syncLoadOK_; 
};

}
