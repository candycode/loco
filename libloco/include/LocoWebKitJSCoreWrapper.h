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

#include <QObject>
#include <QString>
#include <QVariant>
#include <QPointer>
#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebFrame>
#include "LocoWebPage.h"

#include "LocoIJSInterpreter.h"

namespace loco {

class WebKitJSCoreWrapper : public IJSInterpreter {
    Q_OBJECT
public:
    WebKitJSCoreWrapper( QWebPage* wp = 0 ) : wp_( wp ), wf_( 0 ) {
        if( wp ) SetWebPage( wp );
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
    QString Name() const { return "webkit-js-core-wrapper"; }
    void SetWebPage( QWebPage* wp ) { 
        wp_ = wp;
        wf_ = wp_->mainFrame();
        connect( wf_,  SIGNAL( javaScriptWindowObjectCleared() ),
                 this, SIGNAL( JavaScriptContextCleared() ) );
    }
    bool SetAllowInterrupt( bool yes ) {
        qobject_cast< WebPage* >( wp_ )->SetAllowInterruptJavaScript( yes );
        return true;
    }
    bool GetAllowInterrupt() const {
        return qobject_cast< WebPage* >( wp_ )->GetAllowInterruptJavaScript();
    }
signals:
    void JavaScriptContextCleared(); // = javaScriptWindowObjectCleared()
private:
    QPointer< QWebPage > wp_;
    QPointer< QWebFrame > wf_;
};

}
