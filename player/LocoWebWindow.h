#pragma once
//#SRCHEADER

#include <QString>
#include <QList>
#include <QtWebKit/QWebView>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebHistoryItem>
#include <QtWebKit/QWebSettings>

#include "LocoObject.h"
#include "LocoContext.h"
#include "LocoWebView.h"
#include "LocoWebKitAttributeMap.h"
#include "LocoWebKitJSCoreWrapper.h"

namespace loco {

class WebWindow : public Object {
    Q_OBJECT

public:
    WebWindow() : Object( 0, "LocoWebWindow", "Loco/GUI/Window" ) {
        wf_ = webView_.page()->mainFrame(); 
		connect( &webView_, SIGNAL( closing() ),
			     this, SLOT( OnClose() ) );
        connect( webView_.page(), SIGNAL( loadFinished( bool ) ), this, SIGNAL( loadFinished( bool ) ) );
		connect( webView_.page(), SIGNAL( loadProgress( int ) ), this, SIGNAL( loadProgress( int ) ) );
		connect( webView_.page(), SIGNAL( loadStarted() ), this, SIGNAL( loadStarted() ) );
		connect( webView_.page(), SIGNAL( linkClicked( const QUrl& ) ), this, SIGNAL( linkClicked( const QUrl& ) ) );
		connect( wf_, SIGNAL( urlChanged( const QUrl& ) ), this, SIGNAL( urlChanged( const QUrl& ) ) );
	    connect( wf_, SIGNAL( titleChanged( const QString& ) ), this, SIGNAL( titleChanged( const QString& ) ) );
		connect( webView_.page(), SIGNAL( selectionChanged() ), this, SIGNAL( selectionChanged() ) );
		jsInterpreter_->SetWebPage( webView_.page() );   
		ctx_.Init( jsInterpreter_ );
		ctx_.SetJSContextName( "wctx" ); //web window context
		ctx_.AddContextToJS();
    }

	void AddSelfToJSContext( const QString& n = QString() ) {
        jsInterpreter_->AddObjectToJS( n.isEmpty() ? name() : n, this );
    }
    
    void SetNetworkAccessManager( QNetworkAccessManager* nam ) {
	   webView_.page()->setNetworkAccessManager( nam );
    }

private slots:
   
	void OnClose() { emit closing(); }
    
public slots:
	void addSelfToJSContext( const QString& n = QString() ) { AddSelfToJSContext( n ); }
	void setEnableContextMenu( bool yes ) { webView_.EatContextMenuEvent( !yes ); }
	bool getEnableContextMenu() const { return webView_.EatingContextMenuEvent(); }
    QString frameName() const { return wf_->frameName(); }
	//connect from parent context Context::onError() -> WebWindow::onParentError()
    void onParentError( const QString& err ) {
        wf_->evaluateJavaScript( "throw " + err + ";\n" );
    }  
    void load( const QString& url ) { webView_.load( url ); }
    bool isModified() { return webView_.isModified(); }
    //QList< QWebHistoryItem > history();
    void setHtml( const QString& html, const QString& baseUrl = "" ) { webView_.setHtml( html, QUrl( baseUrl ) ); }
    void setZoomFactor( qreal zf ) { webView_.setZoomFactor( zf ); }
    qreal zoomFactor() const { return webView_.zoomFactor(); }
    void setTextSizeMultiplier( qreal tm ) { webView_.setTextSizeMultiplier( tm ); }
    qreal textSizeMultiplier() const { return webView_.textSizeMultiplier(); }
    void setWindowTitle( const QString& t ) { webView_.setWindowTitle( t ); }
    QString title() const { return webView_.title(); }
    void reload() { webView_.reload(); }
    void showFullScreen() { webView_.showFullScreen(); }
    void showNormal() { webView_.showNormal(); }
    void show() { webView_.show(); }
    //QVariant geometry() const;
    //void setGeometry( const QVariant& g );
    void setContentEditable( bool yes ) { webView_.page()->setContentEditable( yes ); }
    qint64 totalBytes() const { return webView_.page()->totalBytes(); }
    void setAttribute( const QString& attr, bool on ) {
    	webView_.page()->settings()->setAttribute( attrMap_[ attr ], on );
    }
    bool testAttribute( const QString& attr) const {
    	return webView_.page()->settings()->testAttribute( attrMap_[ attr ] );
    }
    void setAttributes( const QVariantMap& attr ) {
    	QWebSettings& s = *( webView_.page()->settings() );
    	for( QVariantMap::const_iterator i = attr.begin();
             i != attr.end(); ++i ) {
            s.setAttribute( attrMap_[ i.key() ], i.value().toBool() );
    	}
    }
    QVariant getAttributes() const {
    	const QWebSettings& s = *( webView_.page()->settings() );
    	AttributeTextMap::const_iterator i = attrMap_.GetAttributeToTextMap().begin();
    	const AttributeTextMap::const_iterator e = attrMap_.GetAttributeToTextMap().end();
    	QVariantMap m;
    	for( ; i != e; ++i ) {
    		m[ i.value() ] = s.testAttribute( i.key() );
    	}
    	return m;
    }
    //void setLocalStoragePath( const QString& sp );
    //QString localStoragePath() const;
    //void enablePersistentStorage( const QString& );
    //void setMaximumPagesInCache( int );
    //void setOfflineStoragePath( const QString& );
    //void setOfflineStorageQuota( quint64 );
    //quint64 offlineStorageQuota() const;
    //QString offlineStoragePath() const;
    //void setOfflineWebApplicationCachePath( const QString& );
    //setOfflineWebApplicationCachePath( const QString& );
    //void setOfflineWebApplicationCacheQuota( qint64 );
    QVariant eval( const QString& code, const QStringList& filters = QStringList() ) {
       return ctx_.Eval( code, filters );
    }  
  
    void addParentObjectsToJS() { 
		ctx_.SetAddObjectsFromParentContext( true );
	}

	QString toHtml() const { return wf_->toHtml(); }

	QString selectedText() const { return webView_.page()->selectedText(); }

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
	void loadFinished( bool ok );
    void loadProgress( int );
    void loadStarted();
    void selectionChanged();
    void titleChanged( const QString& );
    void urlChanged( const QUrl& );
	void closing();
private:
    WebView webView_;
	Context ctx_; // this is where objects are created
    QWebFrame* wf_; 
    WebKitAttributeMap attrMap_;
	QSharedPointer< WebKitJSCoreWrapper > jsInterpreter_;
};

}
