#pragma once
//#SRCHEADER

#include <QString>
#include <QtWebKit/QWebFrame>

#include "LocoFilter.h"

namespace loco {

class ScriptFilter : public Filter {
    Q_OBJECT
public:
    ScriptFilter( QWebFrame* wf,
                  const QString& jcode,
                  const QString& jerrfun = "",
                  const QString& codePlaceHolder = "" ) 
        : wf_( wf ), jcode_( jcode ),
          jerrfun_( jerrfun ), codePlaceHolder_( codePlaceHolder ) {}
    void SetWF( QWebFrame* wf ) { wf_ = wf; }
    QWebFrame* GetWF() const { return wf_; }
    void SetJCode( const QString& f ) { jcode_ = f; }
    const QString& GetJCode() const { return jcode_; }
    void SetJErrCode( const QString& f ) { jerrfun_ = f; }
    const QString& GetJErrCode() const { return jerrfun_; }
public:
    QByteArray Apply( const QByteArray& ) const { return QByteArray(); } 
    QString Apply( const QString& s ) const {
        QVariant r;
        // no placeholder, assume it's a function call
        if( codePlaceHolder_.isEmpty() ) {
            r = wf_->evaluateJavaScript( jcode_ + '(' + s + ");" );
        // placeholder, replace with translated code
        } else {
            QString nj = jcode_;
            const QString& njref = nj.replace( codePlaceHolder_, s );
            r = wf_->evaluateJavaScript( njref );
        }
        if( r.isNull() || !r.isValid() ) {
            QVariant e = wf_->evaluateJavaScript( jerrfun_ );
            if( !e.isNull() && e.isValid() ) error( e.toString() );
            else error( "Error parsing code" );
            return s;
        }
        if( r.isValid() ) return r.toString();
        else return "";
    }
    ~ScriptFilter() {}
private:
    QWebFrame* wf_;
    QString jcode_;
    QString jerrfun_;
    QString codePlaceHolder_;
};


}