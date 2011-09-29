#pragma once
//#SRCHEADER

#include <QtWebKit/QWebView>
#include <QKeyEvent>
#include <QMouseEvent>

class QWebPluginFactory;

class WebView : public QWebView {
	Q_OBJECT
public:
	WebView() : eatContextMenuEvent_( true ), eatKeyEvents_( true ), eatMouseEvents_( false ) {}
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
};

