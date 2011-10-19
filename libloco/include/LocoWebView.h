#pragma once
//#SRCHEADER

#include <QtWebKit/QWebView>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPixmap>
#include <QPainter>
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



class QWebPluginFactory;

namespace loco {

typedef QList< QPair< QRegExp, QString > > UrlAgentMap;

class WebPage : public QWebPage {
	Q_OBJECT
public:
	WebPage( QWidget* parent = 0 ) : QWebPage( parent ), allowInterrupt_( true ) {}
    void SetUserAgentForUrl( const QRegExp& url, const QString& userAgent ) {
    	urlAgents_.push_back( qMakePair( url, userAgent ) );
    }
    void SetAllowInterruptJavaScript( bool yes ) {
    	allowInterrupt_ = yes;
    }
protected:
    QString userAgentForUrl( const QUrl& url ) const {
    	const QString s = url.toString();
    	for( UrlAgentMap::const_iterator i = urlAgents_.begin();
    		 i != urlAgents_.end(); ++i ) {
    		if( i->first.exactMatch( s ) ) return i->second;
    	}
        return QWebPage::userAgentForUrl( url );
    }
public slots:
    bool shouldInterruptJavaScript() {
    	if( !allowInterrupt_ ) return false;
    	return QWebPage::shouldInterruptJavaScript();
    }
private:
    bool allowInterrupt_;
    UrlAgentMap urlAgents_;
};


class WebView : public QWebView {
	Q_OBJECT
public:
	WebView() : eatContextMenuEvent_( true ), eatKeyEvents_( true ),
	            eatMouseEvents_( false ), syncLoadOK_( false ) {
		WebPage* wp = new WebPage;
		wp->setParent( this );
		setPage( wp );
		connect( wp, SIGNAL( downloadRequested( const QNetworkRequest& ) ),
				 this, SLOT( OnDownloadRequested( const QNetworkRequest& ) ) );
		connect( wp, SIGNAL( unsupportedContent( QNetworkReply* ) ), this,
				 SLOT( OnUnsupportedContent( QNetworkReply* ) ) );
		wp->setForwardUnsupportedContent( true );
	}
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
    bool SyncLoad( const QString& urlString, const QVariantMap& opt, int timeout ) {
        return SyncLoad( ConfigureURL( urlString, opt ), timeout );
    }
    void Load( const QString& urlString, const QVariantMap& opt ) {
        load( ConfigureURL( urlString, opt ) );
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

private slots:
    void OnLoadFinished( bool ok ) { syncLoadOK_ = ok; }
    void OnDownloadRequested( const QNetworkRequest& nr ) {
    	emit downloadRequested( nr.url().toLocalFile() );
    }
    void OnUnsupportedContent( QNetworkReply* nr ) {
    	emit unsupportedContent( nr->url().toString() );
    }
private:
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
    	QUrl url( urlString );
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
		emit closing();
		QWebView::closeEvent( e );
	}
	void contextMenuEvent( QContextMenuEvent* e ) {
		if( !eatContextMenuEvent_ ) QWebView::contextMenuEvent( e );
	}
	void keyPressEvent( QKeyEvent* ke ) {
		emit keyPress( ke->key(), ke->modifiers(), ke->count() );
		if( !eatKeyEvents_ ) QWebView::keyPressEvent( ke );
	}
	void keyReleaseEvent( QKeyEvent* ke ) {
		emit keyPress( ke->key(), ke->modifiers(), ke->count() );
		if( !eatKeyEvents_ ) QWebView::keyReleaseEvent( ke );
	}
    void mouseMoveEvent( QMouseEvent* me ) {
        emit mouseMove( me->globalX(),
                        me->globalY(),
                        me->x(),
                        me->y(),
                        bool( me->buttons() & Qt::LeftButton ),
                        bool( me->buttons() & Qt::MiddleButton ),
                        bool( me->buttons() & Qt::RightButton ) );
       if( !eatMouseEvents_ ) QWebView::mouseMoveEvent( me );  
    }
    void mousePressEvent( QMouseEvent* me ) {
        emit mousePress( me->globalX(),
                         me->globalY(),
                         me->x(),
                         me->y(),
                         bool( me->buttons() & Qt::LeftButton ),
                         bool( me->buttons() & Qt::MiddleButton ),
                         bool( me->buttons() & Qt::RightButton ) );
        if( !eatMouseEvents_ ) QWebView::mousePressEvent( me );  
    }
    void mouseReleaseEvent( QMouseEvent* me ) {
        emit mouseRelease( me->globalX(),
                           me->globalY(),
                           me->x(),
                           me->y(),
                           bool( me->buttons() & Qt::LeftButton ),
                           bool( me->buttons() & Qt::MiddleButton ),
                           bool( me->buttons() & Qt::RightButton ) );
       if( !eatMouseEvents_ ) QWebView::mouseReleaseEvent( me );  
    }
    void mouseDoubleClickEvent( QMouseEvent* me ) {
        emit mouseDoubleClick( me->globalX(),
                               me->globalY(),
                               me->x(),
                               me->y(),
                               bool( me->buttons() & Qt::LeftButton ),
                               bool( me->buttons() & Qt::MiddleButton ),
                               bool( me->buttons() & Qt::RightButton ) );
        if( !eatMouseEvents_ ) QWebView::mouseDoubleClickEvent( me );  
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
private:
	bool eatContextMenuEvent_;
	bool eatKeyEvents_;
    bool eatMouseEvents_;
    mutable bool syncLoadOK_;
};
}
