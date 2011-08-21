#include <QtWebKit/QWebView>
#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebFrame>
#include <QApplication>
#include <QFile>

#include <iostream>

#include "LocoStdout.h"
#include "LocoWebView.h"


//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
    QWebView* webview = new QWebView;
	if( argc < 2 ) {
		std::cout << "usage: " << argv[ 0 ] << " <.js file >" << std::endl;
		return 1;
	}
	QFile jsource( argv[ 1 ] );
	jsource.open( QIODevice::ReadOnly );
	const QString j = jsource.readAll();
	jsource.close();	
#ifndef LOCO_RELEASE	
	webview->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
#endif
	//webview->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
	std::cout << argv[ 1 ] << ' ' << jsource.readAll().constData() << std::endl;
	//webview->load(   ("<html><body></body></html>");
	webview->page()->settings()->setAttribute( QWebSettings::JavascriptCanOpenWindows, true );
	webview->settings()->setAttribute( QWebSettings::JavascriptEnabled, true );
	webview->settings()->setAttribute( QWebSettings::JavascriptCanOpenWindows, true );
	webview->settings()->setAttribute( QWebSettings::LocalContentCanAccessFileUrls, true );
	webview->settings()->setAttribute( QWebSettings::LocalContentCanAccessRemoteUrls, true );
	
	//std::cout << std::boolalpha << ( webview->page()->currentFrame() == webview->page()->mainFrame() ) << std::endl;
	struct LocoWebConfigDefault : loco::IWebConfig {
		void Shutdown( QWebViewPtr ) {}
		void Init( QWebViewPtr webview ) {
			QWebFrame* f = webview->page()->mainFrame();
			lwv_.Set( webview );
			lwv_.AddConfig( "default", this );
			f->addToJavaScriptWindowObject("locowkit__", &lwv_  );
	        f->addToJavaScriptWindowObject("lococout__", &cout_ );
	        f->evaluateJavaScript( "Loco = function() { "
		                           "this.cout = lococout__;"
                                   "this.web  = locowkit__;"
								   "return this;          }" );
		}
		loco::WebView lwv_;
		loco::Stdout  cout_;
	} locoConfig;

	
    locoConfig.Init( webview );
	webview->page()->mainFrame()->evaluateJavaScript( j );
	//webview->page()->mainFrame()->load( QString( "http://www.google.com" ) );
	webview->show();
	
	
	
	//QString ej = QString( "wkit.evaluateJavaScript(" ) + j + ")";
	//webview->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
	
	//QVariant d = webview->page()->mainFrame()->evaluateJavaScript( j ); 
	//std::cout << d.toDouble() << std::endl;
	
	/* run the message loop */
	return app.exec();
}		