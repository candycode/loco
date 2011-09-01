#pragma once
//#SRCHEADER

#include <QObject>
#include <QtWebKit/QWebView>
#include <QtWebKit/QWebFrame>
#include <QApplication>
#include "LocoStdout.h"
#include <QVariantMap>
#include <QWaitCondition>
#include <QMutex>

const QString SLEEP =           "  this.sleep = function (milliseconds) {"
                                "    var start = new Date().getTime();"
                                "    for (var i = 0; i < 1e10; i++) {"
                                "      if ((new Date().getTime() - start) > milliseconds){"
                                "        break;"
	                            "      }"
                                "    }";
class JEnvConfig : public QObject {
    Q_OBJECT
public:
	JEnvConfig( QWebView* wv, QApplication* a ) : wv_( wv ), a_( a ) {
	    connect( wv->page()->mainFrame(), SIGNAL( javaScriptWindowObjectCleared() ), this, SLOT( addObjectsToJScriptContext() ) );
		jsCode_ = "Loco = function() { "
		                        "  this.cout = lococout__;"
								"  this.env  = locoenv__; "
                               	"  return this; }";
		                       
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
		if( ok ) {
			if( !jsCBack_.isEmpty() ) wv_->page()->mainFrame()->evaluateJavaScript( jsCBack_ ); ///*"loco.cout.println(\"page loaded\");"*/);
		}
		else std::cerr << "error loading page\n";
	}
	void sleep(int ms) {
      QWaitCondition sleep;
	  QMutex m;
	  m.lock();
      sleep.wait(&m,ms);   // two seconds
	}
    QVariant eval( const QString& s ) {
        QVariant v = wv_->page()->mainFrame()->evaluateJavaScript( 
            "try {\n" + s + "\n} catch(e) {\nlocoenv_.riseError( e.description );\n}" );
        return v;
    }
    void riseError( const QString& s ) { std::cerr << s.toStdString() << std::endl; }

private:
	QWebView* wv_;
	QApplication* a_;
	loco::Stdout cout_;
	QString jsCode_;
	mutable QString jsCBack_;
	
};
