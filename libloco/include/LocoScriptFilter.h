#pragma once
//#SRCHEADER
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
				  const QString& jcode = "", 
                  const QString& jerrfun = "",
                  const QString& codePlaceHolder = "" ) 
        : jsInterpreter_( jsInterpreter ), jfun_( jfun), jcode_( jcode ),
          jerrfun_( jerrfun ), codePlaceHolder_( codePlaceHolder ) {}
    void SetJSIntepreter( IJSInterpreter* jsInterpreter ) { jsInterpreter_ = jsInterpreter; }
    IJSInterpreter* GetJSInterpreter() const { return jsInterpreter_; }
    void SetJCode( const QString& f ) { jcode_ = f; }
    const QString& GetJCode() const { return jcode_; }
    void SetJErrCode( const QString& f ) { jerrfun_ = f; }
    const QString& GetJErrCode() const { return jerrfun_; }
public:
	QByteArray Apply( const QByteArray& ba ) const { return Apply( QString( ba ) ).toAscii().constData();  } 
    QString Apply( const QString& ss ) const {
        QString s = ss.trimmed() + "\n";
		s = "\"" + s.replace( "\"", "\\\"" ) + "\"";
        s.replace("\n", "\\n");
		if( !jcode_.isEmpty() ) {
			jsInterpreter_->EvaluateJavaScript( jcode_ );
		}
		QVariant r;
		// no placeholder, assume it's a function call
        if( codePlaceHolder_.isEmpty() ) {
            r = jsInterpreter_->EvaluateJavaScript( jfun_ + "(" +   s  + ");" );
        // placeholder, replace with translated code
        } else {
            QString nj = jfun_;
            const QString& njref = nj.replace( codePlaceHolder_, s );
            r = jsInterpreter_->EvaluateJavaScript( njref );
        }
        if( r.isNull() || !r.isValid() ) {
            QVariant e = jsInterpreter_->EvaluateJavaScript( jerrfun_ );
            if( !e.isNull() && e.isValid() ) error( e.toString() );
            else error( "Error parsing code" );
            return s;
        }
        if( r.isValid() ) return r.toString();
        else return "";
    }
    ~ScriptFilter() {}
private:
	QPointer< IJSInterpreter > jsInterpreter_;
	QString jfun_; 
    QString jcode_;
    QString jerrfun_;
    QString codePlaceHolder_;
};


}
