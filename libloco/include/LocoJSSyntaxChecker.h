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


#include <QtScript/QScriptEngine>
#

#include "LocoObject.h"

namespace loco {

class SyntaxChecker : public Object {
    Q_OBJECT
public:
    SyntaxChecker() : Object( 0, "LocoSyntaxChecker", "Loco/SyntaxChecker" ),
    checkResult_( QScriptEngine::checkSyntax("") ) {}
public slots:
    bool incomplete( const QString& code ) const {
    	checkResult_ = QScriptEngine::checkSyntax( code );
    	return checkResult_.state() == QScriptSyntaxCheckResult::Intermediate;
    }
    bool check( const QString& code ) const {
    	checkResult_ = QScriptEngine::checkSyntax( code );
    	return checkResult_.state() == QScriptSyntaxCheckResult::Valid;
    }
    QString errorMessage()  const { return checkResult_.errorMessage(); }
    int errorLineNumber()   const { return checkResult_.errorLineNumber(); }
    int errorColumnNumber() const { return checkResult_.errorColumnNumber(); }
    QString state() const {
    	if( checkResult_.state() == QScriptSyntaxCheckResult::Error ) return "error";
    	else if( checkResult_.state() == QScriptSyntaxCheckResult::Intermediate ) return "incomplete";
    	else if( checkResult_.state() == QScriptSyntaxCheckResult::Valid ) return "valid";
    	else return "";
    }
    QVariantMap info() const {
    	QVariantMap m;
    	m[ "error"  ] = checkResult_.errorMessage();
    	m[ "line"   ] = checkResult_.errorLineNumber();
    	m[ "column" ] = checkResult_.errorColumnNumber();
    	if( checkResult_.state() == QScriptSyntaxCheckResult::Intermediate ) {
    		m[ "state" ] = "incomplete";
    	} else if( checkResult_.state() == QScriptSyntaxCheckResult::Error ) {
    		m[ "state" ] = "error";
        } else if( checkResult_.state() == QScriptSyntaxCheckResult::Valid ) {
        	m[ "state" ] = "valid";
        }
    	return m;
    }
    QVariantMap checkSyntax( const QString& code ) const { check( code ); return info(); }
private:
    mutable QScriptSyntaxCheckResult checkResult_;
};

}
