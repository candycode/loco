#pragma once
//#SRCHEADER
#include <QMainWindow>
#include <QString>
#include <QVariantMap>
#include <QMenu>
#include <QAction>
#include <QSignalMapper>
#include <QMenuBar>
#include <QStatusBar>
#include <QMap>
#include <QtWebkit/QWebPage>
#include <QtWebkit/QWebFrame>


#include "LocoObject.h"
#include "LocoContext.h"
#include "LocoWebView.h"

namespace loco {

typedef QMap< QString, QMenu* > MenuMap;
typedef QMap< QString, QAction* > ActionMap;
typedef QMap< QAction*, QString > ActionPath;
typedef QMap< QAction*, QString > CBackMap;


class WebMainWindow : public Object {
    Q_OBJECT
public:
	WebMainWindow() : Object( 0, "LocoWebMainWindow", "Loco/GUI/Window" ), webView_( new WebView() )  {
		wf_ = webView_->page()->mainFrame();
	    mw_.setCentralWidget( webView_ );
		webView_->setParent( &mw_ );
		mapper_ = new QSignalMapper( this );
		connect( mapper_, SIGNAL( mapped( QObject* ) ), this, SLOT( ActionTriggered( QObject* ) ) );
		connect( wf_, SIGNAL( javaScriptWindowObjectCleared () ), this, SLOT( JSContextCleared() ) );
		connect( webView_, SIGNAL( closing() ), this, SLOT( OnClose() ) );
		connect( webView_->page(), SIGNAL( loadFinished( bool ) ), this, SIGNAL( loadFinished( bool ) ) );
		connect( webView_->page(), SIGNAL( loadProgress( int ) ), this, SIGNAL( loadProgress( int ) ) );
    }
   void AddSelfToJSContext() {
        wf_->addToJavaScriptWindowObject( name(), this );
   } 
    
   void AddParentObjects() {
        GetContext()->AddJSStdObjects( wf_ );
   }

   void SetNetworkAccessManager( QNetworkAccessManager* nam ) {
	   webView_->page()->setNetworkAccessManager( nam );
   }

private slots:
    void JSContextCleared() {
        if( addParentObjs_ ) {
            AddParentObjects();
            wf_->evaluateJavaScript( GetContext()->GetJSInitCode() );
        }
        if( addSelf_ ) AddSelfToJSContext();
    }
	void OnClose() { emit closing(); }
public slots:
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
    void setWindowTitle( const QString& t ) { mw_.setWindowTitle( t ); }
    QString title() const { return mw_.windowTitle(); }
    void showNormal() { mw_.showNormal(); }
    void showFullScreen() { mw_.showFullScreen(); }
    void show() {  mw_.show(); }
	//connect from parent context Context::onError() -> WebWindow::onParentError()
    void onParentError( const QString& err ) {
        wf_->evaluateJavaScript( "throw " + err + ";\n" );
    }  
    void load( const QString& url ) { webView_->load( url ); }
    bool isModified() { return webView_->isModified(); }
    //QList< QWebHistoryItem > history();
    void setHtml( const QString& html ) { webView_->setHtml( html ); }
    void setZoomFactor( qreal zf ) { webView_->setZoomFactor( zf ); }
    qreal zoomFactor() const { return webView_->zoomFactor(); }
    void setTextSizeMultiplier( qreal tm ) { webView_->setTextSizeMultiplier( tm ); }
    qreal textSizeMultiplier() const { return webView_->textSizeMultiplier(); }
    void reload() { webView_->reload(); }
    //QVariant geometry() const;
    //void setGeometry( const QVariant& g );
    void setContentEditable( bool yes ) { webView_->page()->setContentEditable( yes ); }
    qint64 totalBytes() const { return webView_->page()->totalBytes(); }
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
	QString toHtml() const { return wf_->toHtml(); }


signals:
    void actionTriggered( const QString& path );

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
  			   QString cback = i.value().toMap()[ "cback" ].toString();
  			   printf( "%s\n", cback.toStdString().c_str());
  		       if( !cback.isEmpty() ) cbacks_[ a ] = cback;
  		       actions_[ path ] = a;
  		       actionPath_[ a ] = path;
  		   } else {
  			   QMenu* m = 0;
  		       if( parent != 0 ) {
  			       m = parent->addMenu( i.key() );
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
			if( i != cbacks_.end() ) wf_->evaluateJavaScript( cbacks_[ a ] );
            else emit actionTriggered( actionPath_[ a ] );
        }
    }
signals:
    //void linkClicked( const QString& );
    void loadFinished( bool );
    void loadProgress( int );
    //void loadStarted();
    //void selectionChanged();
    //void statusBarMessage( const QString& );
    //void titleChanged( const QString& );
    //void urlChanged( const QString& );
    // other option:
    // 1) connect javaScriptContextReset to parent context
    // 2) have parent context initialize this object as needed
    //void javaScriptContextReset( this );
	void closing();
private:
   WebView* webView_;
   bool addSelf_;
   bool addParentObjs_;
   QWebFrame* wf_;
   QMainWindow mw_;
   MenuMap menuItems_;
   ActionMap actions_;
   CBackMap cbacks_;
   ActionPath actionPath_;
   QSignalMapper* mapper_;
};

}
