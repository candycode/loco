#pragma once
//#SRCHEADER

#include <QtWebKit/QWebView>

class WebView : public QWebView {
	Q_OBJECT
protected:
	void closeEvent( QCloseEvent* e ) {
		emit closing();
		QWebView::closeEvent( e );
	}
	void contextMenuEvent( QContextMenuEvent* ) {}
signals:
	void closing();
};

