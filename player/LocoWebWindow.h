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

namespace loco {

class WebWindow : public Object {
    Q_OBJECT

public:
    WebWindow() : Object( 0, "LocoWebWindow", "Loco/GUI/Window" ) {
        wf_ = webView_.page()->mainFrame(); 
        connect( wf_, SIGNAL( javaScriptWindowObjectCleared () ),
                 this, SLOT( JSContextCleared() ) );
		connect( &webView_, SIGNAL( closing() ),
			     this, SLOT( OnClose() ) );
        connect( webView_.page(), SIGNAL( loadFinished( bool ) ), this, SIGNAL( loadFinished( bool ) ) );
		connect( webView_.page(), SIGNAL( loadProgress( int ) ), this, SIGNAL( loadProgress( int ) ) );
		connect( webView_.page(), SIGNAL( loadStarted() ), this, SIGNAL( loadStarted() ) );
		connect( webView_.page(), SIGNAL( linkClicked( const QUrl& ) ), this, SIGNAL( linkClicked( const QUrl& ) ) );
		connect( wf_, SIGNAL( urlChanged( const QUrl& ) ), this, SIGNAL( urlChanged( const QUrl& ) ) );
	    connect( wf_, SIGNAL( titleChanged( const QString& ) ), this, SIGNAL( titleChanged( const QString& ) ) );
		connect( webView_.page(), SIGNAL( selectionChanged() ), this, SIGNAL( selectionChanged() ) );
    }

    void AddSelfToJSContext() {
        wf_->addToJavaScriptWindowObject( name(), this );
    }
    
    void AddParentObjects() {
        GetContext()->AddJSStdObjects( wf_ );
    }

    void SetNetworkAccessManager( QNetworkAccessManager* nam ) {
	   webView_.page()->setNetworkAccessManager( nam );
    }

private slots:
    void JSContextCleared() {
        if( addParentObjs_ ) {
            AddParentObjects();
            wf_->evaluateJavaScript( GetContext()->GetJSInitCode() );
        }
        if( addSelf_ ) AddSelfToJSContext();
        if( !preLoadCBack_.isEmpty() ) wf_->evaluateJavaScript( preLoadCBack_ );
    }
	void OnClose() { emit closing(); }
    
public slots:
	
    //connect from parent context Context::onError() -> WebWindow::onParentError()
    void onParentError( const QString& err ) {
        wf_->evaluateJavaScript( "throw " + err + ";\n" );
    }  
    void load( const QString& url ) { webView_.load( url ); }
    bool isModified() { return webView_.isModified(); }
    //QList< QWebHistoryItem > history();
    void setHtml( const QString& html ) { webView_.setHtml( html ); }
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
        return wf_->evaluateJavaScript( GetContext()->Filter( code, filters ) ); 
    }  
    void setAddSelfToJS( bool yes ) { addSelf_ = yes; }
    void setAddParentObjectsToJS( bool yes ) { addParentObjs_ = yes; }
	QString toHtml() const { return wf_->toHtml(); }
	void setPreLoadCBack( const QString& code, const QStringList& filters = QStringList() ) {
	    preLoadCBack_ = GetContext()->Filter( code, filters );
	}
	QString selectedText() const { return webView_.page()->selectedText(); }

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
    QWebFrame* wf_; 
    bool addSelf_;
    bool addParentObjs_;
    QString preLoadCBack_;
    WebKitAttributeMap attrMap_;
};

}
