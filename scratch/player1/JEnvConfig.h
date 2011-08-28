#pragma once
//#SRCHEADER

#include <QObject>
#include <QtWebKit/QWebView>
#include <QtWebKit/QWebFrame>
#include <QApplication>
#include "LocoStdout.h"
#include <QVariantMap>

class JEnvConfig : public QObject {
    Q_OBJECT
public:
	JEnvConfig( QWebView* wv, QApplication* a ) : wv_( wv ), a_( a ) {
	    connect( wv->page()->mainFrame(), SIGNAL( javaScriptWindowObjectCleared() ), this, SLOT( addObjectsToJScriptContext() ) );
		jsCode_ = "Loco = function() { "
		                        "  this.cout = lococout__;"
								"  this.env  = locoenv__; "
								"  return this; }         ";
        connect( wv->page()->mainFrame(), SIGNAL( loadFinished(bool) ), this, SLOT( loadFinished(bool)));
	}
public:
	const QString& jsInit() const { return jsCode_; }
public slots:
	void addObjectsToJScriptContext() {
		QWebFrame* wf = wv_->page()->mainFrame();
		wf->addToJavaScriptWindowObject( "locoenv__", this );
        //wf_->addToJavaScriptWindowObject( wv_, "locowv__" );
		//wf->addToJavaScriptWindowObject( "locowf__", wv_->page()->mainFrame() );
		wf->addToJavaScriptWindowObject( "lococout__",  &cout_ );
		//std::cout << jsCode_.toStdString() << std::endl;
        wf->evaluateJavaScript( jsCode_ );
		std::cout << "addObjectsToJScriptContext()\n";
	}
	void showWindow() { 
		wv_->show();
		std::cout << "showWindow()\n";
	}
	void showFullScreen() { 
		wv_->showFullScreen();
		std::cout << "showWindow()\n";
	}
	int exec() { 
		return a_->exec();
		std::cout << "exec()\n";
	}
	void load( const QString& url, const QString& jsCBack ) {
		jsCBack_ = jsCBack;
		std::cout << jsCBack_.toStdString() << std::endl;
		wv_->load( url );

		std::cout << "load()\n";
	}
	void loadFinished( bool ok ) {
	    addObjectsToJScriptContext();
		if( ok ) wv_->page()->mainFrame()->evaluateJavaScript( jsCBack_ ); ///*"loco.cout.println(\"page loaded\");"*/);
		else std::cerr << "error loading page\n";
	}

private:
	QWebView* wv_;
	QApplication* a_;
	loco::Stdout cout_;
	QString jsCode_;
	mutable QString jsCBack_;
	
};
