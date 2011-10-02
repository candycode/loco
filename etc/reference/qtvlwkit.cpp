

#include <QtWebKit/QWebView>
#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebPluginFactory>
#include <QtGui/QVBoxLayout>
#include <QtCore/QList>
#include "VLWindow.h"

#include <iostream>

using namespace vl;
using namespace vlQt4;

static const char* VLMIMETYPE = "application/x-qt-plugin";

//------------------------------------------------------------------------------
class VLPluginFactory : public QWebPluginFactory {
public:
	VLPluginFactory( QObject* parent = 0 ) : QWebPluginFactory( parent ) {}
	virtual QObject* create ( const QString & mimeType, 
				  			  const QUrl & url, 
							  const QStringList& argumentNames, 
							  const QStringList& argumentValues ) const { 
		if( mimeType != VLMIMETYPE ) return 0;
		VLWindow* vl = new VLWindow;
		int w = 400;
		int h = 400;
		if( argumentNames.indexOf( "width" ) != -1 ) {
			const QString& width = argumentValues.at( argumentNames.indexOf( "width" ) );
			bool result = false;
			w = width.toInt( &result, 10 );
		}
		if( argumentNames.indexOf( "height" ) != -1 ) {
			const QString& height = argumentValues.at( argumentNames.indexOf( "height" ) );
			bool result = false;
			h = height.toInt( &result, 10 );
		}
		vl->resize( w, h );
		return vl;
	}

	virtual QList<Plugin> plugins () const {
		QWebPluginFactory::MimeType mt;
		mt.description = "Viz library sample integration with WebKit";
		mt.fileExtensions << ".none";
		mt.name = VLMIMETYPE;
		QList<QWebPluginFactory::MimeType> mtl;
		mtl.push_back( mt );
		QWebPluginFactory::Plugin p;
		p.description = "VL plugin";
		p.mimeTypes = mtl;
		p.name = "VL WIDGET";//VLMIMETYPE;
		QList< QWebPluginFactory::Plugin > pl;
		pl.push_back( p );
		return pl;
	}
	virtual void refreshPlugins () {}
};
static const char* VLWEBPAGE =  "<html><body><h1>HTML Previewer</h1>"
								" <p>This example shows you how to use QWebView to"
								" preview HTML data written in a QPlainTextEdit.</p>"
								" <object type=\"application/x-qt-plugin\" width=\"1200\" height=\"900\"></object>"
								" </body></html>";
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
    QWebView* webview = new QWebView;
	webview->page()->setPluginFactory( new VLPluginFactory );
	/* show the window */
	
	//webview->page()->currentFrame()->evaluateJavaScript( "<script src=\"file:///C:/projects/architecture/embedding/vlwebkit/view/lib/jquery-1.5.1.min.js\"></script>" );
	if( argc == 1 ) webview->/*page()->currentFrame()->*/setHtml( VLWEBPAGE );
	else webview->page()->currentFrame()->load( QString( argv[ 1 ] ) ); 
	webview->page()->settings()->setAttribute( QWebSettings::JavascriptCanOpenWindows, true );
	webview->page()->mainFrame()->addToJavaScriptWindowObject("wkit", webview->page()->mainFrame() ); 
	webview->resize(1024,768);
	//webview->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
	webview->show();
	/* run the Win32 message loop */
	int val = app.exec();

	return val;
}

