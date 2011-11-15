#pragma once
//#SRCHEADER

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
