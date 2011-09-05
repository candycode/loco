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
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

class JEnvConfig : public QObject {
    Q_OBJECT
public:
    JEnvConfig( QWebView* wv, QApplication* a ) : wv_( wv ), a_( a ), redirectCnt_( 0 ) {
        connect( wv->page()->mainFrame(), SIGNAL( javaScriptWindowObjectCleared() ), this, SLOT( addObjectsToJScriptContext() ) );
        connect( wv->page()->mainFrame(), SIGNAL( loadFinished(bool) ), this, SLOT( loadFinished(bool) ) );
         connect( &netAccessMgrRead_, SIGNAL( finished( QNetworkReply* ) ),
                 this, SLOT( readFromUrlFinished( QNetworkReply* ) ) ); 
        ///@warning it seems that the objects passed from Qt cannot be set directly into the prototype
        jsCode_ = "Loco = function() {" 
                  "  this.cout = lococout__;"
                  "  this.env  = locoenv__;"
                  "  return this; }\n"
                  "Loco.prototype.p=function(v){this.cout.println(v);}\n"  
                  "Loco.prototype.cbacks = [];\n"
                  "Loco.prototype.load = function(url,cb) {"
                  "  this.cbacks.push(cb);"
                  "  this.env.set(url); }\n"
                  "Loco.prototype.read = function(url,cb) {"
                  "  this.cbacks.push(cb);"
                  "  this.env.readFromUrl(url); }\n";
    }
public:
    const QString& jsInit() const { return jsCode_; }
public slots:

    void readFromUrl( const QString& url ) {
        //readFromUrlMutex_.lock();
        netAccessMgrRead_.get( QNetworkRequest( QUrl( url ) ) );
        //while( !r->isReadable() );
        //QString s = r->readAll();
        //while( r->bytesAvailable() > 0 ) s.append( r->readAll() ); 
    }

     void readFromUrlFinished( QNetworkReply* nr ) {
       readFromUrlString_ = "";
       if( nr->error() != QNetworkReply::NoError ) {
           std::cerr << nr->errorString().toStdString() << std::endl;
       } else {
          std::cout << "READ" << std::endl;
          //while( nr->bytesAvailable() > 0 ) {  
          readFromUrlString_ = nr->readAll();
          //}
       }
       nr->close();
       QString j = "(Loco.prototype.cbacks.pop())(" + readFromUrlString_ + QString( ");" );
       std::cout << j.toStdString() << std::endl;
       //wv_->page()->mainFrame()->evaluateJavaScript( j ); 
       wv_->page()->mainFrame()->evaluateJavaScript( "document.write(\"" + readFromUrlString_ + "\");");
       //readFromUrlWaitCond_.wakeAll(); 
    }

    void addObjectsToJScriptContext() {
        QWebFrame* wf = wv_->page()->mainFrame();
        wf->addToJavaScriptWindowObject( "locoenv__", this );
        //wf_->addToJavaScriptWindowObject( wv_, "locowv__" );
        //wf->addToJavaScriptWindowObject( "locowf__", wv_->page()->mainFrame() );
        wf->addToJavaScriptWindowObject( "lococout__",  &cout_ );
        //std::cout << jsCode_.toStdString() << std::endl;
        if( init_ ) {
            wf->evaluateJavaScript( jsCode_ );
            init_ = false;
        } 
         
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
    void setHtml( const QString& html ) { 
        wv_->setHtml( html );
        std::cout << "showWindow()\n";
    }
    int exec() { 
        return a_->exec();
        std::cout << "exec()\n";
    }

    QString read( const QString& url ) {
        if( redirectCnt_ > 1 ) return "";
        QNetworkAccessManager *networkMgr = new QNetworkAccessManager(this);
        QNetworkReply *reply = networkMgr->get( QNetworkRequest( QUrl( url ) ) );
 
        QEventLoop loop;
        QObject::connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
 
        // Execute the event loop here, now we will wait here until readyRead() signal is emitted
        // which in turn will trigger event loop quit.
        loop.exec();
        QVariant possibleRedirectUrl =  reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        QUrl urlRedirectedTo = redirectUrl(possibleRedirectUrl.toUrl(), urlRedirectedTo);
        
        // If the URL is not empty, we're being redirected.
        if(!urlRedirectedTo.isEmpty()) {
            ++redirectCnt_;
            std::cout << urlRedirectedTo.toString().toStdString() << std::endl;
            return read( urlRedirectedTo.toString() );
        }
        else { redirectCnt_ = 0; return reply->readAll(); }
    }
    
    QString escapeQuotes( QString s ) {
        //s.replace( "\"", "\\\"" ); 
        return "\"" + s + "\"";
    }

    QUrl redirectUrl(const QUrl& possibleRedirectUrl, const QUrl& oldRedirectUrl) const {
        QUrl redirectUrl;
        if(!possibleRedirectUrl.isEmpty() && possibleRedirectUrl != oldRedirectUrl)
            redirectUrl = possibleRedirectUrl;
        return redirectUrl;
    }

    void load( const QString& url, const QString& jsCBack ) {
        jsCBack_ = jsCBack;
        std::cout << jsCBack_.toStdString() << std::endl;
        wv_->load( url );
        //wv_->setUrl( url );
        std::cout << "load()\n";
    }
    void set( const QString& url ) {
        std::cout << "set()\n";
        this->load( url );
        
    }
    void loadFinished( bool ok ) {
        std::cout << "loadFinished" << std::endl; 
        if( ok ) {
            if( !jsCBack_.isEmpty() ) {
                std::cout << "Executing callback javascript code" << std::endl;
                wv_->page()->mainFrame()->evaluateJavaScript( jsCBack_ );
            } else {
                std::cout << "Invoking callback..." << std::endl;
                wv_->page()->mainFrame()->evaluateJavaScript( 
                    "loco.cout.println(Loco.prototype.cbacks.length);(Loco.prototype.cbacks.pop())();" );
            }
        }
        else std::cerr << "error loading page\n";
    }
    void sleep(int ms) {
      sleepMutex_.lock();
      sleepWCond_.wait( &sleepMutex_, ms );   // two seconds
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
    mutable int redirectCnt_;
};
