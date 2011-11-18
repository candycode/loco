#pragma once
//#SRCHEADER
#include <QtGui/QMainWindow>
#include <QString>
#include <QVariantMap>
#include <QtGui/QMenu>
#include <QMap>
#include <QtGui/QPixmap>
#include <QtGui/QBitmap>
#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebSettings>
#include <QtWebKit/QWebElementCollection>
#include <QtWebKit/QWebElement>
#include <QtGui/QCursor>


#include "LocoObject.h"
#include "LocoContext.h"
#include "LocoWebView.h"
#include "LocoWebKitAttributeMap.h"
#include "LocoWebKitJSCoreWrapper.h"
#include "LocoDynamicWebPluginFactory.h"
#include "LocoStaticWebPluginFactory.h"
#include "LocoNetworkAccessManager.h"
#include "LocoWebElement.h"
#include "LocoWrappedWidget.h"


class QWebPluginFactory;

namespace loco {

typedef QMap< QString, QMenu* > MenuMap;
typedef QMap< QString, QAction* > ActionMap;
typedef QMap< QAction*, QString > ActionPath;
typedef QMap< QAction*, QString > CBackMap;


class WebWindow : public WrappedWidget {
    Q_OBJECT
private:
	struct ObjectEntry {
	    QObject* obj_;
		QString jsName_;
		bool own_;
		ObjectEntry( QObject* obj, const QString& jsName, bool own ) :
		    obj_( obj ), jsName_( jsName ), own_( own ) {}
	};
	typedef QList< ObjectEntry > ContextObjects;
public:
    WebWindow() : WrappedWidget( 0, "LocoWebWindow", "Loco/GUI/Window" ),
        webView_( new WebView() ), jsInterpreter_( new WebKitJSCoreWrapper )  {
        wf_ = webView_->page()->mainFrame();
        ws_ = webView_->page()->settings();
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
        connect( webView_, SIGNAL( unsupportedContent( const QString& )  ),
        		 this, SIGNAL( unsupportedContent( const QString& ) ) );
        connect( webView_, SIGNAL( downloadRequested( const QString& ) ),
                 this, SIGNAL( downloadRequested( const QString& ) ) );
		connect( webView_, SIGNAL( fileDownloadProgress( qint64, qint64 ) ), this, SIGNAL( fileDownloadProgress( qint64, qint64 ) ) );
		connect( webView_, SIGNAL( actionTriggered( const QString&, bool ) ), this, SIGNAL( webActionTriggered( const QString&, bool ) ) );
		connect( webView_, SIGNAL( fileDownloadProgress( qint64, qint64 ) ), this, SIGNAL( fileDownloadProgress( qint64, qint64 ) ) );
		connect( webView_, SIGNAL( JavaScriptConsoleMessage( const QString&, int, const QString& ) ),
		   		 this, SIGNAL( javaScriptConsoleMessage( const QString&, int, const QString& ) ) );
		connect( webView_, SIGNAL( actionTriggered( const QString&, bool ) ), this, SIGNAL( webActionTriggered( const QString&, bool ) ) );
		connect( webView_->page(), SIGNAL( statusBarMessage( const QString& ) ), this, SIGNAL( statusBarMessage( const QString& ) ) );
        connect( webView_->page(), SIGNAL( statusBarVisibilityChangeRequested( bool ) ),
        		 this, SIGNAL( statusBarVisibilityChangeRequested( bool ) ) );
        connect( webView_->page(), SIGNAL( toolBarVisibilityChangeRequested( bool ) ),
                 this, SIGNAL( toolBarVisibilityChangeRequested( bool ) ) );
		connect( webView_->page(), SIGNAL( linkHovered( const QString&, const QString&, const QString& ) ),
        		 this, SIGNAL( linkHovered( const QString&, const QString&, const QString& ) ) );
        connect( jsInterpreter_, SIGNAL( JavaScriptContextCleared() ), this, SLOT( JavaScriptContextCleared() ) );
        jsInterpreter_->setParent( this );
        jsInterpreter_->SetWebPage( webView_->page() );   
        ctx_.Init( jsInterpreter_ );
        ctx_.SetJSContextName( "wctx" ); //web window context
        ctx_.AddContextToJS();
        // base class instance is constructed before current instance, it is therefore
        // not possible to connect signals in base class constructor since Widget() method
        // must return pointer which is set within this constructor
        WrappedWidget::ConnectSignals();
    }
	QWidget* Widget() { return webView_; }
	virtual const QWidget* Widget() const { return webView_; }
    ~WebWindow() { if( webView_ && webView_->parent() == 0 ) webView_->deleteLater(); }

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

    WebView* GetWebView() const { return webView_; }

    void SetWebPluginFactory( ::QWebPluginFactory* wpf ) { webView_->page()->setPluginFactory( wpf ); }

    ::QWebPluginFactory* GetWebPluginFactory() const { return webView_->page()->pluginFactory(); }

private slots:
    void JavaScriptContextCleared() {
		for( ContextObjects::const_iterator i = ctxObjects_.begin();
			 i != ctxObjects_.end(); ++i ) {
		   ctx_.AddQObjectToJSContext( i->obj_, i->jsName_, i->own_ ); 
		}	
	}
    void PreLoadCBack() { ctx_.Eval( preLoadCBack_ ); }

public slots:
	void setPreLoadCBack( const QString& cback ) { preLoadCBack_ = cback; }
	void addObjectToContext( QObject* obj, const QString& jsName, bool own = false ) {
		ctx_.AddQObjectToJSContext( obj, jsName, own );
		ctxObjects_.push_back( ObjectEntry( obj, jsName, own ) );
	}
	void resetObject() { ctxObjects_.clear(); }
	void setEmitWebActionSignal( bool yes ) { webView_->SetEmitWebActionSignal( yes ); }
	void triggerAction( const QString& action, bool checked = false ) {
		if( !webView_->TriggerAction( action, checked ) ) error( "Cannot trigger action " + action );
	}
    void syncLoad( const QUrl& url, int timeout ) { webView_->SyncLoad( url, timeout ); }
    void load( const QUrl& url ) { webView_->load( url ); }
    void setLinkDelegationPolicy( const QString& p ) {
    	if( p == "all" ) webView_->page()->setLinkDelegationPolicy( QWebPage::DelegateAllLinks );
    	else if( p == "external" ) webView_->page()->setLinkDelegationPolicy( QWebPage::DelegateExternalLinks );
    	else if( p == "none ") webView_->page()->setLinkDelegationPolicy( QWebPage::DontDelegateLinks );
    	else error( "Wrong link delegation policy '" + p +"'" );
    }
    bool saveUrl( const QString& url, const QString& filename, int timeout ) { return webView_->SaveUrl( url, filename, timeout ); }
    QString webKitVersion() const { return QTWEBKIT_VERSION_STR; }

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
    	if( wec.count() < 1 ) return QList< QVariant >();
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
    bool syncLoad( const QString& url, int timeout ) { return webView_->SyncLoad( url,  timeout ); }
    bool syncLoad( const QString& urlString, const QVariantMap& opt, int timeout ) {
        return webView_->SyncLoad( urlString, opt, timeout );
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
    void setEnableContextMenu( bool yes ) { webView_->EatContextMenuEvent( !yes ); }
    bool getEnableContextMenu() const { return !webView_->EatingContextMenuEvent(); }
    void setForwardKeyEvents( bool yes ) { webView_->EatKeyEvents( !yes ); }
    bool getForwardKeyEvents( bool yes ) const { return !webView_->EatingKeyEvents(); }
    //connect from parent context Context::onError() -> WebWindow::onParentError()
    void onParentError( const QString& err ) {
        ctx_.Eval( "throw " + err + ";\n" );
    }  
    void load( const QString& url ) { webView_->Load( url ); }
    bool isModified() { return webView_->isModified(); }
    //QList< QWebHistoryItem > history();
    void setHtml( const QString& html, const QString& baseUrl = "" ) { webView_->setHtml( html, QUrl( baseUrl ) ); }
    void setBaseUrl( const QString& url ) { webView_->page()->mainFrame()->setUrl( url ); }
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
    void selectionChanged();
    void titleChanged( const QString& );
    void urlChanged( const QUrl& );
    void onRequest( const QVariantMap& );
    void keyPress( int key, int modifiers, int count );
    void keyRelease( int key, int modifiers, int count );
    void downloadRequested( const QString& );
    void unsupportedContent( const QString& );
    void webActionTriggered( const QString&, bool );
    void fileDownloadProgress( qint64, qint64 );
    void javaScriptConsoleMessage( const QString&, int, const QString& );
	void statusBarMessage( const QString& );
    void statusBarVisibilityChangeRequested( bool );
    void toolBarVisibilityChangeRequested( bool );
private:
    WebView* webView_; //owned by this object
    Context ctx_; // this is where objects are created
    QPointer< QWebFrame > wf_; //owned by webview
    QWebSettings* ws_; //owned by web frame; not a QObject: cannot wrap with a QPointer
    WebKitAttributeMap attrMap_;
    WebKitJSCoreWrapper* jsInterpreter_;
    QString preLoadCBack_;
	ContextObjects ctxObjects_;
};

}
