#pragma once
//#SRCHEADER

#include <QString>
#include <QList>
#include <QtWebkit/QWebView>
#include <QtWebkit/QWebFrame>
#include <QtWebkit/QWebHistoryItem>

#include "LocoObject.h"
#include "LocoContext.h"

namespace loco {

class WebWindow : public Object {
    Q_OBJECT

public:
   WebWindow() : Object( 0, "LocoWebWindow", "Loco/GUI/Window" ) {
        wf_ = webView_.page()->mainFrame(); 
        connect( wf_, SIGNAL( javaScriptWindowObjectCleared () ),
                 this, SLOT( JSContextCleared() ) ); 

   }

   void AddSelfToJSContext() {
        wf_->addToJavaScriptWindowObject( name(), this );
   } 
    
   void AddParentObjects() {
        GetContext()->AddJSStdObjects( wf_ );
   }

private slots:
    void JSContextCleared() {
        if( addParentObjs_ ) {
            AddParentObjects();
            wf_->evaluateJavaScript( GetContext()->GetJSInitCode() );
        }
        if( addSelf_ ) AddSelfToJSContext();
    } 
    
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
    //void setAttribute( const QString&, bool );
    //bool testAtribute( const QString& ) const;
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
signals:
    //void linkClicked( const QString& );
    //void loadFinished( bool );
    //void loadProgress( int );
    //void loadStarted();
    //void selectionChanged();
    //void statusBarMessage( const QString& );
    //void titleChanged( const QString& );
    //void urlChanged( const QString& );
    // other option:
    // 1) connect javaScriptContextReset to parent context
    // 2) have parent context initialize this object as needed
    //void javaScriptContextReset( this );  
private:
    QWebView webView_;
    QWebFrame* wf_; 
    bool addSelf_;
    bool addParentObjs_;
};

}