#pragma once
//#SRCHEADER

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

class QWebPluginFactory;

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
    void SaveSnapshot( const QString& filePath, int quality = -1 ) const {
        QPixmap p( page()->viewportSize() );
       	QPainter painter( &p );
        page()->mainFrame()->render( &painter, QWebFrame::ContentsLayer );
        p.save( filePath, 0, quality );
    }
    QPixmap Snapshot() const {
	    QPixmap p( page()->viewportSize() );
	    QPainter painter( &p );
	    page()->mainFrame()->render( &painter, QWebFrame::ContentsLayer );
	    return p;
    }
    QString SavePDF( const QString& filePath, const QVariantMap& options = QVariantMap() ) const;
    bool SyncLoad( const QUrl& url, int timeout ) {
        QEventLoop loop;
    	QObject::connect( this, SIGNAL( loadFinished( bool ) ), this, SLOT( OnLoadFinished( bool ) ) );
    	syncLoadOK_ = false;
       	load( url );
        // soft real-time guarantee: kill network request if the total time is >= timeout
    	QTimer::singleShot( timeout, &loop, SLOT( quit() ) );
    	// Execute the event loop here, now we will wait here until readyRead() signal is emitted
    	// which in turn will trigger event loop quit.
    	loop.exec();
    	QObject::disconnect( this, SIGNAL( loadFinished( bool ) ), this, SLOT( OnLoadFinished( bool ) ) );
    	return syncLoadOK_;
    }
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
private:
    QUrl TranslateUrl( const QString& urlString ) {
    	if( urlString.contains( "://" ) ) return QUrl( urlString );
    	else if( urlString.startsWith( '/' ) ) return QUrl( "file://" + urlString );
    	else return QUrl( "file://" + QDir::currentPath() + "/" + urlString );
    }
    void AddToUrlQuery( QUrl& url, const QVariantMap& q ) {
        for( QVariantMap::const_iterator i = q.begin(); i != q.end(); ++i ) {
        	if( i.value().type() != QVariant::List ) {
    	        url.addQueryItem( i.key(), i.value().toString() );
        	} else {
        		QVariantList m = i.value().toList();
        		for( QVariantList::const_iterator k = m.begin(); k != m.end(); ++k ) {
        			url.addQueryItem( i.key(), ( *k ).toString() );
        		}
        	}
    	}
    }
    QUrl ConfigureURL( const QString& urlString, const QVariantMap& opt ) {
    	QUrl url( TranslateUrl( urlString ) );
    	if( opt.contains( "query_delimiters" ) ) {
    		url.setQueryDelimiters( opt[ "query_delimiters" ].toList().at( 0 ).toChar().toAscii(),
    				                opt[ "query_delimiters" ].toList().at( 1 ).toChar().toAscii() );
    	}
		if( opt.contains( "username" ) ) url.setUserName( opt[ "username" ].toString() );
		if( opt.contains( "password" ) ) url.setPassword( opt[ "password" ].toString() );
		if( opt.contains( "port" ) ) url.setPort( opt[ "port" ].toInt() );
		if( !opt.contains( "query" ) ) return url;
		QVariantMap q = opt[ "query" ].toMap();
		AddToUrlQuery( url, q );
		return url;
    }
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
