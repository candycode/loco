#pragma once
////////////////////////////////////////////////////////////////////////////////
//Copyright (c) 2012, Ugo Varetto
//All rights reserved.
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions are met:
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder nor the
//      names of its contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//DISCLAIMED. IN NO EVENT SHALL UGO VARETTO BE LIABLE FOR ANY
//DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////


///\todo replace std::cerr with exceptions ?
#include <iostream>

#include <QString>
#include <QVariant>
#include <QPointer>
#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebSettings>

#include "LocoIJSInterpreter.h"

namespace loco {

class JSCoreWebPage : public QWebPage {
    Q_OBJECT
public:
	JSCoreWebPage( QObject* parent = 0 ) : QWebPage( parent ), allowInterrupt_( true ) {}
protected:
	void javaScriptConsoleMessage( const QString& t, int l, const QString& s ) {
		QWebPage::javaScriptConsoleMessage( t, l, s );
		emit JavaScriptConsoleMessage( t, l, s );
	}
public:
	void SetAllowInterrupt( bool yes ) { allowInterrupt_ = yes; }
	bool GetAllowInterrupt() const { return allowInterrupt_; }
	bool supportsExtension( Extension extension ) const { 
	    if( extension == QWebPage::ErrorPageExtension ) { 
		    return true; 
        } 
		return false;
	}
	bool extension(Extension extension, const ExtensionOption *option = 0, ExtensionReturn *output = 0) {
        if( extension != QWebPage::ErrorPageExtension ) return false;

        ErrorPageExtensionOption *errorOption = ( ErrorPageExtensionOption* ) option;
        std::cerr << "Error loading " << qPrintable(errorOption->url.toString())  << std::endl;
        if(errorOption->domain == QWebPage::QtNetwork)
            std::cerr << "Network error (" << errorOption->error << "): ";
        else if(errorOption->domain == QWebPage::Http)
            std::cerr << "HTTP error (" << errorOption->error << "): ";
        else if(errorOption->domain == QWebPage::WebKit)
            std::cerr << "WebKit error (" << errorOption->error << "): ";

        std::cerr << qPrintable(errorOption->errorString) << std::endl;

        return false;
    }
public slots:
    bool shouldInterruptJavaScript() {
    	if( !allowInterrupt_ ) return false;
    	return QWebPage::shouldInterruptJavaScript();
    }
signals:
	void JavaScriptConsoleMessage( const QString&, int, const QString& );
private:
	bool allowInterrupt_;
};


class WebKitJSCore : public IJSInterpreter {
	Q_OBJECT
public:
	WebKitJSCore() : wp_( new JSCoreWebPage )  {
		wf_ = wp_->mainFrame();
		connect( wf_, SIGNAL( javaScriptWindowObjectCleared() ),
			     this, SIGNAL( JavaScriptContextCleared() ) );
		connect( wp_, SIGNAL( JavaScriptConsoleMessage( const QString&, int, const QString& ) ),
			     this, SIGNAL( JavaScriptConsoleMessage( const QString&, int, const QString& ) ) );
        wp_->setParent( this );
	}
	QVariant EvaluateJavaScript( const QString& code ) {
		return wf_->evaluateJavaScript( code );
        
	}
	void AddObjectToJS( const QString& name, QObject* obj ) {
		wf_->addToJavaScriptWindowObject( name, obj );
	}
	void AddObjectToJS( const QString& name, QObject* obj, QScriptEngine::ValueOwnership vo ) {
		wf_->addToJavaScriptWindowObject( name, obj, vo );
	}
	void Init() {}
	QString Name() const { return "webkit-js-core"; }
	bool SetAllowInterrupt( bool yes ) { wp_->SetAllowInterrupt( yes ); return true; }
	bool GetAllowInterrupt() const { return wp_->GetAllowInterrupt(); }
signals:
	void JavaScriptContextCleared(); // = javaScriptWindowObjectCleared()
	void JavaScriptConsoleMessage( const QString, int, const QString& );
private:
    JSCoreWebPage* wp_;
    QPointer< QWebFrame > wf_;
};

}
