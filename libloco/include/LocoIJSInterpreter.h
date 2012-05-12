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
#include <QtScript/QScriptEngine>

namespace loco {

//derivation from QObject required to store pointers into QPointers

class IJSInterpreter : public QObject {
    Q_OBJECT
public:
    virtual QVariant EvaluateJavaScript( const QString& code ) = 0;
    virtual void AddObjectToJS( const QString& name, QObject* obj, QScriptEngine::ValueOwnership vo ) = 0;
    virtual void AddObjectToJS( const QString& name, QObject* obj ) = 0;
    virtual void Init() = 0;
    virtual QString Name() const = 0; //used to pass information on type and version e.g. QtScript vs Webkit JS core
    virtual bool SetAllowInterrupt( bool  ) = 0; //returns false if not supported
    virtual    bool GetAllowInterrupt() const = 0; //always return false if not supported
    virtual ~IJSInterpreter() {}
signals:
    /*virtual*/ void JavaScriptConsoleMessage( const QString& /*text*/, int /*line*/, const QString& /*source id*/ );
};

}
