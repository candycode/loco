#pragma once
//#SRCHEADER

#include <QtWebKit/QWebView>

class WebView : public QWebView {
	Q_OBJECT
public:
	WebView() : eatContextMenuEvent_( true ) {}
	void EatContextMenuEvent( bool yes ) { eatContextMenuEvent_ = yes; }
	bool EatingContextMenuEvent() const { return eatContextMenuEvent_; }
protected:
	void closeEvent( QCloseEvent* e ) {
		emit closing();
		QWebView::closeEvent( e );
	}
	void contextMenuEvent( QContextMenuEvent* e ) {
		if( !eatContextMenuEvent_ ) QWebView::contextMenuEvent( e );
	}
signals:
	void closing();
private:
	bool eatContextMenuEvent_;
};

