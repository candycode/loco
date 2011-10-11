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

//@todo remove
#include <iostream>

class QWebPluginFactory;

namespace loco {
class WebView : public QWebView {
	Q_OBJECT
public:
	WebView() : eatContextMenuEvent_( true ), eatKeyEvents_( true ),
	            eatMouseEvents_( false ), syncLoadOK_( false ) {}
	void EatContextMenuEvent( bool yes ) { eatContextMenuEvent_ = yes; }
	bool EatingContextMenuEvent() const { return eatContextMenuEvent_; }
	void EatKeyEvents( bool yes ) { eatKeyEvents_ = yes; }
	bool EatingKeyEvents() const { return eatKeyEvents_; }
    void EatMouseEvents( bool yes ) { eatMouseEvents_ = yes; }
    bool EatingMouseEvents() const { return eatMouseEvents_; }
    void SetWebPluginFactory( QWebPluginFactory* pf ) {
        /*QWebPluginFactory* wpf = page()->pluginFactory(); // is plugin factory owned by Page() ?
        if( wpf != 0 && pf == wpf ) return;
        if( wpf != 0 ) {
            wpf_->setParent( 0 );
            wpf_->deleteLater();
        }*/
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
private slots:
    void OnLoadFinished( bool ok ) { syncLoadOK_ = ok; }

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
private:
	bool eatContextMenuEvent_;
	bool eatKeyEvents_;
    bool eatMouseEvents_;
    mutable bool syncLoadOK_;
};
}
