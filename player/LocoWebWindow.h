#pragma once
//#SRCHEADER
#include <QMainWindow>
#include <QString>
#include <QVariantMap>
#include <QMenu>
#include <QMap>
#include <QPixmap>
#include <QBitmap>
#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebSettings>
#include <QCursor>


#include "LocoObject.h"
#include "LocoContext.h"
#include "LocoWebView.h"
#include "LocoWebKitAttributeMap.h"
#include "LocoWebKitJSCoreWrapper.h"
#include "LocoWebMainWindow.h"

#include <iostream>


class QWebPluginFactory;

namespace loco {

typedef QMap< QString, QMenu* > MenuMap;
typedef QMap< QString, QAction* > ActionMap;
typedef QMap< QAction*, QString > ActionPath;
typedef QMap< QAction*, QString > CBackMap;


class WebWindow : public Object {
    Q_OBJECT
public:
    WebWindow() : Object( 0, "LocoWebWindow", "Loco/GUI/Window" ),
        webView_( new WebView() ), jsInterpreter_( new WebKitJSCoreWrapper )  {
        wf_ = webView_->page()->mainFrame();
        ws_ = webView_->page()->settings();
        connect( webView_, SIGNAL( closing() ), this, SLOT( OnClose() ) );
        connect( webView_, SIGNAL( keyPress( int, int, int ) ), this, SIGNAL( keyPress( int, int, int ) ) );
        connect( webView_, SIGNAL( keyRelease( int, int, int ) ), this, SIGNAL( keyRelease( int, int, int ) ) );
        connect( webView_->page(), SIGNAL( loadFinished( bool ) ), this, SIGNAL( loadFinished( bool ) ) );
        connect( webView_->page(), SIGNAL( loadProgress( int ) ), this, SIGNAL( loadProgress( int ) ) );
        connect( webView_->page(), SIGNAL( loadStarted() ), this, SIGNAL( loadStarted() ) );
        connect( webView_->page(), SIGNAL( linkClicked( const QUrl& ) ), this, SIGNAL( linkClicked( const QUrl& ) ) );
        connect( wf_, SIGNAL( urlChanged( const QUrl& ) ), this, SIGNAL( urlChanged( const QUrl& ) ) );
        connect( wf_, SIGNAL( titleChanged( const QString& ) ), this, SIGNAL( titleChanged( const QString& ) ) );
        connect( webView_->page(), SIGNAL( selectionChanged() ), this, SIGNAL( selectionChanged() ) );
        connect( &ctx_, SIGNAL( JSContextCleared() ), this, SLOT( PreLoadCBack() ) );

        jsInterpreter_->setParent( this );
        jsInterpreter_->SetWebPage( webView_->page() );   
        ctx_.Init( jsInterpreter_ );
        ctx_.SetJSContextName( "wctx" ); //web window context
        ctx_.AddContextToJS();
        
    }
    void AddSelfToJSContext() {
        ctx_.AddJSStdObject( this );
    }
    
    void SetNetworkAccessManager( QNetworkAccessManager* nam ) {
        webView_->page()->setNetworkAccessManager( nam );
    }

    void SetContext( Context* ctx ) {
        Object::SetContext( ctx );
        ctx_.SetParentContext( GetContext() );
    }

    WebView* GetWebView() const { return webView_; }

    void SetWebPluginFactory( QWebPluginFactory* wpf ) { webView_->SetWebPluginFactory( wpf ); }

    QWebPluginFactory* GetWebPluginFactory() const { return webView_->GetWebPluginFactory(); }

private slots:

    void PreLoadCBack() { ctx_.Eval( preLoadCBack_ ); }
    void OnClose() { emit closing(); }

public slots:

    void setPreLoadCBack( const QString& cback ) { preLoadCBack_ = cback; }

public slots:

    void setMouseCursor( const QString& shape ) {
        if( shape == "arrow" ) webView_->setCursor( Qt::ArrowCursor );
        else if( shape == "upArrow" ) webView_->setCursor( Qt::UpArrowCursor );
        else if( shape == "cross" )  webView_->setCursor( Qt::CrossCursor );
        else if( shape == "wait" ) webView_->setCursor( Qt::WaitCursor );
        else if( shape == "ibeam" ) webView_->setCursor( Qt::IBeamCursor );
        else if( shape == "sizeVert" ) webView_->setCursor( Qt::SizeVerCursor );
        else if( shape == "sizeHor" ) webView_->setCursor( Qt::SizeHorCursor );
        else if( shape == "wait" ) webView_->setCursor( Qt::WaitCursor );
        else if( shape == "sizeAll" ) webView_->setCursor( Qt::SizeAllCursor );
        else if( shape == "hide" ) webView_->setCursor( Qt::BlankCursor );
        else if( shape == "splitVert" ) webView_->setCursor( Qt::SplitVCursor );
        else if( shape == "splitHor" ) webView_->setCursor( Qt::SplitHCursor );
        else if( shape == "pointHand" ) webView_->setCursor( Qt::PointingHandCursor );
        else if( shape == "forbid" ) webView_->setCursor( Qt::ForbiddenCursor );
        else if( shape == "openHand" ) webView_->setCursor( Qt::OpenHandCursor );
        else if( shape == "closedHand" ) webView_->setCursor( Qt::ClosedHandCursor );
        else if( shape == "whatsThis" ) webView_->setCursor( Qt::WhatsThisCursor );
        else if( shape == "busy" ) webView_->setCursor( Qt::BusyCursor );
        else if( shape == "dragMove" ) webView_->setCursor( Qt::DragMoveCursor );
        else if( shape == "dragCopy" ) webView_->setCursor( Qt::DragCopyCursor );
        else if( shape == "dragLink" ) webView_->setCursor( Qt::DragLinkCursor ); 
    }
    void setMouseCursor( const QPixmap& pmap, int hotX = -1, int hotY = -1 ) {
        QCursor c( pmap, hotX, hotY );
        webView_->setCursor( c );
    }
    void setWindowAttributes( Qt::WidgetAttribute wa ) {
        webView_->setAttribute( wa );
    }
    void setWindowIcon( const QPixmap& p ) { webView_->setWindowIcon( p ); }
    void setWindowIcon( const QString& f ) { webView_->setWindowIcon( QIcon( f ) ); }  
    void move( int x, int y ) { webView_->move( x, y ); }
    void setParentWindow( QObject* obj ) {
        if( qobject_cast< WebWindow* >( obj ) != 0  ) {
            WebWindow* ww = qobject_cast< WebWindow* >( obj );     
            webView_->setParent( ww->GetWebView() );
        } else if( qobject_cast< WebMainWindow* >( obj ) != 0 ) {
            WebMainWindow* ww = qobject_cast< WebMainWindow* >( obj );     
            webView_->setParent( ww->GetMainWindow() );
        } else {
            error( "Not a LoCO window object" );
        }          
    }   
    void setWindowType( const QStringList& flags ) {
        Qt::WindowFlags w = 0;
        for( QStringList::const_iterator i = flags.begin(); i != flags.end(); ++i ) {
            const QString& f = *i;
            if( f == "dialog" ) w |= Qt::Dialog;
            else if( f == "window" ) w |= Qt::Window;
            else if( f == "sheet" ) w |= Qt::Sheet;
            else if( f == "drawer" ) w |= Qt::Drawer;
            else if( f == "popup" ) w |= Qt::Popup;
            else if( f == "tool" ) w |= Qt::Tool;
            else if( f == "splash" ) w |= Qt::SplashScreen;
            else if( f == "sub" ) w |= Qt::SubWindow;     
        }
        webView_->setWindowFlags( w );
    }
    void setWindowOpacity( double op ) { webView_->setWindowOpacity( op ); }
    double windowOpacity() const { return webView_->windowOpacity(); }
    void setMask( const QPixmap& p ) {
        webView_->setMask( p.mask() );
    }
    void setMask( const QString& ppath ) {
        QPixmap p( ppath );
        webView_->setMask( p.mask() );
    }  
    void enableAction( QWebPage::WebAction action, bool yes ) { webView_->pageAction( action )->setEnabled( yes ); }
    bool isActionEnabled( QWebPage::WebAction action ) const { return webView_->pageAction( action )->isEnabled(); }
    void triggerAction( QWebPage::WebAction action, bool checked = false ) { webView_->triggerPageAction( action, checked ); }
    void setName( const QString& n ) { Object::setName( n ); }
    void addSelfToJSContext() { AddSelfToJSContext(); }
    QString frameName() const { return wf_->frameName(); }
    void setEnableContextMenu( bool yes ) { webView_->EatContextMenuEvent( !yes ); }
    bool getEnableContextMenu() const { return !webView_->EatingContextMenuEvent(); }
    void setForwardKeyEvents( bool yes ) { webView_->EatKeyEvents( !yes ); }
    bool getForwardKeyEvents( bool yes ) const { return !webView_->EatingKeyEvents(); }
    void setWindowTitle( const QString& t ) { webView_->setWindowTitle( t ); }
    QString title() const { return webView_->windowTitle(); }
    void showNormal() { webView_->showNormal(); }
    void showFullScreen() { webView_->showFullScreen(); }
    void show() {  webView_->show(); }
    //connect from parent context Context::onError() -> WebWindow::onParentError()
    void onParentError( const QString& err ) {
        ctx_.Eval( "throw " + err + ";\n" );
    }  
    void load( const QString& url ) { webView_->load( url ); }
    bool isModified() { return webView_->isModified(); }
    //QList< QWebHistoryItem > history();
    void setHtml( const QString& html, const QString& baseUrl = "" ) { webView_->setHtml( html, QUrl( baseUrl ) ); }
    void setZoomFactor( qreal zf ) { webView_->setZoomFactor( zf ); }
    qreal zoomFactor() const { return webView_->zoomFactor(); }
    void setTextSizeMultiplier( qreal tm ) { webView_->setTextSizeMultiplier( tm ); }
    qreal textSizeMultiplier() const { return webView_->textSizeMultiplier(); }
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
        ctx_. SetAddObjectsFromParentContext( true );
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
    void loadFinished( bool );
    void loadProgress( int );
    void loadStarted();
    void selectionChanged();
    void titleChanged( const QString& );
    void urlChanged( const QUrl& );
    void closing();
    void keyPress( int key, int modifiers, int count );
    void keyRelease( int key, int modifiers, int count );

public:
    // required because:
    // 1) a QWidget derived object (WebView in this case) cannot have a non-QWidget ('this') as parent
    // 2) we cannot declare webView_ on the stack because it needs to be possible to set it 
    //    as the child of another widget
    ~WebWindow() {
        if( webView_->parent() == 0 ) webView_->deleteLater(); //
    }

private:
    WebView* webView_; //owned by main window
    Context ctx_; // this is where objects are created
    QPointer< QWebFrame > wf_; //owned by webview
    QWebSettings* ws_; //owned by web frame; not a QObject: cannot wrap with a QPointer
    WebKitAttributeMap attrMap_;
    WebKitJSCoreWrapper* jsInterpreter_;
    QString preLoadCBack_;
};

}
