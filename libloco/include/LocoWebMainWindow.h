#pragma once
//#SRCHEADER
#include <limits>

#include <QMainWindow>
#include <QString>
#include <QVariantMap>
#include <QMenu>
#include <QAction>
#include <QSignalMapper>
#include <QMenuBar>
#include <QStatusBar>
#include <QMap>
#include <QPixmap>
#include <QBitmap>
#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebSettings>
#include <QToolBar>
#include <QVariantList>


#include "LocoObject.h"
#include "LocoContext.h"
#include "LocoWebView.h"
#include "LocoWebKitAttributeMap.h"
#include "LocoWebKitJSCoreWrapper.h"
#include "LocoDynamicWebPluginFactory.h"
#include "LocoStaticWebPluginFactory.h"
#include "LocoWebElement.h"

namespace loco {

typedef QMap< QString, QMenu* > MenuMap;
typedef QMap< QString, QAction* > ActionMap;
typedef QMap< QAction*, QString > ActionPath;
typedef QMap< QAction*, QString > CBackMap;


class WebMainWindow : public Object {
    Q_OBJECT
public:
    WebMainWindow() : Object( 0, "LocoWebMainWindow", "Loco/GUI/Window" ),
        webView_( new WebView() ), jsInterpreter_( new WebKitJSCoreWrapper ), toolBar_( 0 )  {
        wf_ = webView_->page()->mainFrame();
        ws_ = webView_->page()->settings();
        mw_.setCentralWidget( webView_ ); // webView_ lifetime managed by mw_;
        mapper_ = new QSignalMapper( this );
        connect( mapper_, SIGNAL( mapped( QObject* ) ), this, SLOT( ActionTriggered( QObject* ) ) );
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

    QMainWindow* GetMainWindow() { return &mw_; }

    void SetWebPluginFactory( QWebPluginFactory* wpf ) { webView_->page()->setPluginFactory( wpf ); }

    QWebPluginFactory* GetWebPluginFactory() const { return webView_->page()->pluginFactory(); }

private slots:

    void PreLoadCBack() { ctx_.Eval( preLoadCBack_ ); }
    void OnClose() { emit closing(); }

public slots:

    void highLightText( const QString& substring ) { webView_->HighlightText( substring ); }

    QList< QVariant > forEachElement( const QString& selectorQuery,
		                          const QString& cond,
		                          int maxNum = std::numeric_limits< int >::max() ) {
	    QWebElementCollection wec = webView_->FindElements( selectorQuery );
	    if( wec.count() < 1 ) return QList< QVariant >();
	    QList< QVariant > we;
	    int n = 0;
	    for( QWebElementCollection::iterator i = wec.begin();
	         i != wec.end() && n < maxNum; ++i, ++n ) {
	        if( ( *i ).evaluateJavaScript( cond ).toBool() ) {
	   	          we.push_back( GetContext()->AddObjToJSContext( new WebElement( *i ) ) );
	        }
        }
        return we;
    }
    QVariant findFirstElement( const QString& selectorQuery ) {
	    QWebElement we = webView_->FindFirstElement( selectorQuery );
	    if( we.isNull() ) return QVariant();
	    return GetContext()->AddObjToJSContext( new WebElement( we ) ) ;
    }
    QList< QVariant > findElements( const QString& selectorQuery ) const {
	    QWebElementCollection wec = webView_->FindElements( selectorQuery );
	    if(  wec.count() < 1 ) return QList< QVariant >();
	    QList< QVariant > we;
	    for(QWebElementCollection::const_iterator i = wec.constBegin();
	        i != wec.constEnd(); ++i ) {
		    we.push_back( GetContext()->AddObjToJSContext( new WebElement( *i ) ) );
	    }
	    return we;
    }
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
    void setUserAgentForUrl( const QRegExp& url, const QString& userAgent ) {
	    webView_->SetUserAgentForUrl( url, userAgent );
    }
    void SetAllowInterruptJavaScript( bool yes ) {
		webView_->SetAllowInterruptJavaScript( yes );
	}
    bool syncLoad( const QString& url, int timeout ) { return webView_->SyncLoad( QUrl( url ), timeout ); }
    bool syncLoad( const QString& urlString, const QVariantMap& opt, int timeout ) {
        return syncLoad( urlString, opt, timeout );
    }
    void load( const QString& urlString, const QVariantMap& opt ) { return webView_->Load( urlString, opt ); }
    void setPageSize( int w, int h ) { webView_->SetPageSize( w, h ); }
    QVariantMap pageSize() const {
    	QSize sz = webView_->PageSize();
    	QVariantMap m;
    	m[ "width"  ] = sz.width();
    	m[ "height" ] = sz.height();
    	return m;
    }
    void saveSnapshot( const QString& filePath, int quality = -1 ) const {
    	webView_->SaveSnapshot( filePath );
    }
    QPixmap snapshot() const {
    	return webView_->Snapshot();
    }
    void saveToPDF( const QString& fname ) const {
    	webView_->SavePDF( fname );
    }

    void createWebPluginFactory( const QString& type = "dynamic" ) {
    	if( type.toLower() == "dynamic" ) {
    		webView_->page()->setPluginFactory( new DynamicWebPluginFactory );
    	} else if( type.toLower() == "static" ) {
    		webView_->page()->setPluginFactory( new StaticWebPluginFactory );
    	}
    }
    void setWindowOpacity( double op ) { mw_.setWindowOpacity( op ); }
    double windowOpacity() const { return mw_.windowOpacity(); }
    void setMask( const QPixmap& p ) {
        mw_.setMask( p.mask() );
    }
    void setMask( const QString& ppath ) {
        QPixmap p( ppath );
        mw_.setMask( p.mask() );
    }  
    void setPreLoadCBack( const QString& cback ) { preLoadCBack_ = cback; }

    void setStatusBarText( const QString& text, int timeout = 0 ) {
        mw_.statusBar()->showMessage( text, timeout );
    }
    void setMenu( const QVariantMap& jsonTree ) { SetMenu( 0, jsonTree, "" ); }
    void setMenuItemProperty( const QString& path, const QString& prop, const QString& value ) {
        MenuMap::iterator mi = menuItems_.find( path );
        if( mi != menuItems_.end() ) {
            SetMenuProperty( mi.value(), prop, value );
        } else {
            ActionMap::iterator ai = actions_.find( path );
            if( ai != actions_.end() ) {
                SetActionProperty( ai.value(), prop, value );
            }
            else error( "Menu item " + path + " not found" );
        }
    }
    QString getMenuItemProperty( const QString& path, const QString& prop ) const {
        MenuMap::const_iterator mi = menuItems_.find( path );
        if( mi != menuItems_.end() ) {
            return GetMenuProperty( mi.value(), prop );
        } else {
            ActionMap::const_iterator ai = actions_.find( path );
            if( ai != actions_.end() ) {
                return GetActionProperty( ai.value(), prop );
            }
            else {
                error( "Menu item " + path + " not found" );
                return "";
            }
        }
    }
public slots:
    void setMouseCursor( const QString& shape ) {
        if( shape == "arrow" ) mw_.setCursor( Qt::ArrowCursor );
        else if( shape == "upArrow" ) mw_.setCursor( Qt::UpArrowCursor );
        else if( shape == "cross" )  mw_.setCursor( Qt::CrossCursor );
        else if( shape == "wait" ) mw_.setCursor( Qt::WaitCursor );
        else if( shape == "ibeam" ) mw_.setCursor( Qt::IBeamCursor );
        else if( shape == "sizeVert" ) mw_.setCursor( Qt::SizeVerCursor );
        else if( shape == "sizeHor" ) mw_.setCursor( Qt::SizeHorCursor );
        else if( shape == "wait" ) mw_.setCursor( Qt::WaitCursor );
        else if( shape == "sizeAll" ) mw_.setCursor( Qt::SizeAllCursor );
        else if( shape == "hide" ) mw_.setCursor( Qt::BlankCursor );
        else if( shape == "splitVert" ) mw_.setCursor( Qt::SplitVCursor );
        else if( shape == "splitHor" ) mw_.setCursor( Qt::SplitHCursor );
        else if( shape == "pointHand" ) mw_.setCursor( Qt::PointingHandCursor );
        else if( shape == "forbid" ) mw_.setCursor( Qt::ForbiddenCursor );
        else if( shape == "openHand" ) mw_.setCursor( Qt::OpenHandCursor );
        else if( shape == "closedHand" ) mw_.setCursor( Qt::ClosedHandCursor );
        else if( shape == "whatsThis" ) mw_.setCursor( Qt::WhatsThisCursor );
        else if( shape == "busy" ) mw_.setCursor( Qt::BusyCursor );
        else if( shape == "dragMove" ) mw_.setCursor( Qt::DragMoveCursor );
        else if( shape == "dragCopy" ) mw_.setCursor( Qt::DragCopyCursor );
        else if( shape == "dragLink" ) mw_.setCursor( Qt::DragLinkCursor ); 
    }
    void setMouseCursor( const QPixmap& pmap, int hotX = -1, int hotY = -1 ) {
        QCursor c( pmap, hotX, hotY );
        mw_.setCursor( c );
    }
    void setToolbar( const QVariantList& jsonData ) {
       if( toolBar_ == 0 ) {
           toolBar_ = new QToolBar;
           mw_.addToolBar( toolBar_ );
       }  
       for( QVariantList::const_iterator i = jsonData.begin(); i != jsonData.end(); ++i ) {
            if( !i->toMap().isEmpty() ) {
                const QVariantMap m = i->toMap();
                if( m[ "type" ].toString() == "separator" ) {
                    toolBar_->addSeparator();
                } else if(  m[ "type"  ].toString() == "button" &&
                           !m[ "icon"  ].toString().isEmpty()   &&
                           !m[ "path"  ].toString().isEmpty()   &&
                           !m[ "cback" ].toString().isEmpty() ) {
                    QAction* a = new QAction( QIcon( m[ "icon" ].toString() ), m[ "text" ].toString(), toolBar_ );
                    a->setToolTip( m[ "tooltip" ].toString() );
                    a->setStatusTip( m[ "status" ].toString() );
                    mapper_->setMapping( a, a );
                    connect( a, SIGNAL( triggered() ), mapper_, SLOT( map() ) );
                    cbacks_[ a ] = m[ "cback" ].toString(); 
                    actions_[ "path" ] = a;
                    actionPath_[ a ] = "path";     
                }
            }            
        }
    }
    void hideToolBar() { toolBar_->hide(); }
    void showToolBar() { toolBar_->show(); }
    void hideMenu() { mw_.menuBar()->hide(); }
    void showMenu() { mw_.menuBar()->show(); }
    void hideStatusBar() { mw_.statusBar()->hide(); }
    void showStatusBar() { mw_.statusBar()->show(); }
    void setWindowIcon( const QPixmap& p ) { webView_->setWindowIcon( p ); }
    void setWindowIcon( const QString& f ) { webView_->setWindowIcon( QIcon( f ) ); }  
    void move( int x, int y ) { mw_.move( x, y ); }
    void enableAction( QWebPage::WebAction action, bool yes ) { webView_->pageAction( action )->setEnabled( yes ); }
    bool isActionEnabled( QWebPage::WebAction action ) const { return webView_->pageAction( action )->isEnabled(); }
    void trriggerAction( QWebPage::WebAction action, bool checked = false ) { webView_->triggerPageAction( action, checked ); }
    void setName( const QString& n ) { Object::setName( n ); }
    void addSelfToJSContext() { AddSelfToJSContext(); }
    void setMenuBarVisibility( bool on ) { if( mw_.menuBar() ) mw_.menuBar()->setVisible( on ); }
    void setStatusBarVisibility( bool on ) { if( mw_.statusBar() ) mw_.statusBar()->setVisible( on ); }
    QString frameName() const { return wf_->frameName(); }
    void setEnableContextMenu( bool yes ) { webView_->EatContextMenuEvent( !yes ); }
    bool getEnableContextMenu() const { return !webView_->EatingContextMenuEvent(); }
    void setForwardKeyEvents( bool yes ) { webView_->EatKeyEvents( !yes ); }
    bool getForwardKeyEvents( bool yes ) const { return !webView_->EatingKeyEvents(); }
    void setWindowTitle( const QString& t ) { mw_.setWindowTitle( t ); }
    QString title() const { return mw_.windowTitle(); }
    void showNormal() { mw_.showNormal(); }
    void showFullScreen() { mw_.showFullScreen(); }
    void show() {  mw_.show(); }
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

private:
   void SetMenu( QMenu* parent, const QVariantMap& jsonTree, QString path ) {
         for( QVariantMap::const_iterator i = jsonTree.begin();
              i != jsonTree.end(); ++i ) {
             const bool leaf = i.value().type() == QVariant::Map &&
                               (i.value().toMap().begin()).value().type() == QVariant::String;
             if( leaf ) {
                 QAction* a = new QAction( i.key(), &mw_ );
                 mapper_->setMapping( a, a );
                 connect( a, SIGNAL( triggered() ), mapper_, SLOT( map() ) );
                 parent->addAction( a );
                 QVariantMap m = i.value().toMap();
                 a->setToolTip( m[ "tooltip" ].toString() );
                 a->setStatusTip( m[ "status" ].toString() );
                 a->setIcon( QIcon( m[ "icon" ].toString() ) );
                 a->setIconText( m[ "icon_text" ].toString() ); 
                 QString cback = m[ "cback" ].toString();
                 if( !cback.isEmpty() ) cbacks_[ a ] = cback;
                 actions_[ path ] = a;
                 actionPath_[ a ] = path;
             } else {
                 QMenu* m = 0;
                 if( parent != 0 ) {
                   const QString entry = i.key();
                   if( !entry.startsWith( "__" ) && !entry.startsWith( "--" ) ) {   
                         m = parent->addMenu( i.key() );
                   } else {
                       parent->addSeparator();
                       continue;
                   }                   
                 } else {
                     m = mw_.menuBar()->addMenu( i.key() );
                 }
                 const QString menuPath = path + "/" + i.key();
                 menuItems_[ menuPath ] = m;
                 SetMenu( m, i.value().toMap(), menuPath );
             }
         }
   }
   void SetMenuProperty( QMenu*, const QString&, const QString& ) {}
   void SetActionProperty( QAction*, const QString&, const QString& ) {}
   QString GetMenuProperty( QMenu*, const QString& ) const { return ""; }
   QString GetActionProperty( QAction*, const QString& ) const { return ""; }

private slots:
    void ActionTriggered( QObject* action ) {
        QAction* a = qobject_cast< QAction* >( action );
        if( !a ) error( "Type mismatch" );
        else {
            CBackMap::const_iterator i = cbacks_.find( a );
            if( i != cbacks_.end() ) ctx_.Eval( cbacks_[ a ] );
            else emit actionTriggered( actionPath_[ a ] );
        }
    }
signals:
    void actionTriggered( const QString& path );
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
private:
    QPointer< WebView > webView_; //owned by main window
    Context ctx_; // this is where objects are created
    QPointer< QWebFrame > wf_; //owned by webview
    QWebSettings* ws_; //owned by web frame; not a QObject cannot wrap into a QPointer
    QMainWindow mw_;
    MenuMap menuItems_;
    ActionMap actions_;
    CBackMap cbacks_;
    ActionPath actionPath_;
    QSignalMapper* mapper_;
    WebKitAttributeMap attrMap_;
    WebKitJSCoreWrapper* jsInterpreter_;
    QString preLoadCBack_;
    QToolBar* toolBar_;    
};

}
