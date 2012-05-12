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

#include <QString>
#include <QPointer>

#include "LocoIJSInterpreter.h"
#include "LocoFilter.h"

namespace loco {

class ScriptFilter : public Filter {
    Q_OBJECT
public:
    ScriptFilter( IJSInterpreter* jsInterpreter,
                  const QString& jfun,
                  const QString& jcodeBegin = "",
                  const QString& jcodeEnd = "",
                  const QString& jerrfun = "",
                  const QString& codePlaceHolder = "" ) 
        : jsInterpreter_( jsInterpreter ), jfun_( jfun), jcodeBegin_( jcodeBegin ),
          jcodeEnd_( jcodeEnd ), jerrfun_( jerrfun ), codePlaceHolder_( codePlaceHolder ) {}
    void SetJSIntepreter( IJSInterpreter* jsInterpreter ) { jsInterpreter_ = jsInterpreter; }
    IJSInterpreter* GetJSInterpreter() const { return jsInterpreter_; }
    void SetJCodeBegin( const QString& b ) { jcodeBegin_ = b; }
    void SetJCodeEnd( const QString& e ) { jcodeEnd_ = e; }
    const QString& GetJCodeBegin() const { return jcodeBegin_; }
    const QString& GetJCodeEnd() const { return jcodeEnd_; }
    void SetJErrCode( const QString& f ) { jerrfun_ = f; }
    const QString& GetJErrCode() const { return jerrfun_; }
public:
    QByteArray Apply( const QByteArray& ba ) const { return Apply( QString( ba ) ).toAscii();  } 
    QString Apply( const QString& ss ) const;
    ~ScriptFilter() {}
private:
    QPointer< IJSInterpreter > jsInterpreter_;
    QString jfun_; 
    QString jcodeBegin_;
    QString jcodeEnd_;
    QString jerrfun_;
    QString codePlaceHolder_;
};


}
