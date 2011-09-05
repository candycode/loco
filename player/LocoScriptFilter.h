#pragma once
//#SRCHEADER

#include <QString>
#include <QtWebKit/QWebFrame>

#include "LocoFilter.h"

namespace loco {

class ScriptFilter : public Filter {
public:
    ScriptFilter( QWebFrame* wf,
                  const QString& s,
                  const QString& jcode,
                  const QString& jerrfun = "",
                  const QString& codePlaceHolder = "" ) 
        : s_( s ), wf_( wf ), jcode_( jcode ),
          jerrfun_( jerrfun ), codePlaceHolder_( codePlaceHolder ) {}
    void SetWF( QWebFrame* wf ) { wf_ = wf; }
    QWebFrame* GetWF() const { return wf_; }
    void SetScript( const QString& s ) { s_ = s; }
    const QString& GetScript() { return s_; }
    void SetJCode( const QString& f ) { jcode_ = f; }
    const QString& GetJCode() const { return jcode_; }
    void SetJErrCode( const QString& f ) { jerrfun_ = f; }
    const QString& GetJErrCode() const { return jerrfun_; }
public:
    QString Filter( const QString& s ) const {
        QVariant r;
        // no placeholder, assume it's a function call
        if( codePlaceHolder_.isEmpty() ) {
            r = wf_->evaluateJavaScript( jcode + '(' + s + ");" );
        // placeholder, replace with translated code
        } else {
            QString nj = jcode_;
            const QString& njref = nj.replace( placeHolder_, s );
            r = wf_->evaluateJavaScript( njref );
        }
        if( r.isNull() || !r.isValid() ) {
            QVariant e = wf_->evaluateJavaScript( jerrfun );
            if( !e.isNull() && e.isValid() ) error( e.toString() );
            else error( "Error parsing code" );
            return s;
        }
        if( r.isValid() ) return r.toString();
        else return "";
    }
private:
    QString s_;
    QWebFrame wf_;
    QString jcode_;
    QString jerrfun_;
    QString codePlaceHolder_;
};


}