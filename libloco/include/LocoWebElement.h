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


#include <limits>

#include <QtWebKit/QWebElement>
#include <QtWebKit/QWebElementCollection>
#include <QString>
#include <QVariant>
#include <QVariantMap>
#include <QList>
#include <QStringList>

#include "LocoObject.h"
#include "LocoContext.h"

namespace loco {

class WebElement : public Object {
    Q_OBJECT
public:
    WebElement( const QWebElement& we ) : we_( we ) {}
    QWebElement& qWebElement() { return we_; }
    const QWebElement& qWebElement() const { return we_; }
public slots:
    QVariant findFirstElement( const QString& selectorQuery ) {
           QWebElement we = we_.findFirst( selectorQuery );
           if( we.isNull() ) return QVariant();
           return GetContext()->AddObjToJSContext( new WebElement( we ) ) ;
    }
    QList< QVariant > forEachElement( const QString& selectorQuery,
                                      const QString& cond,
                                      int maxNum = std::numeric_limits< int >::max() ) {
        QWebElementCollection wec = we_.findAll( selectorQuery );
        if( wec.count() < 1 ) return QList< QVariant >();
        QList< QVariant > we;
        int n = 0;
        for( QWebElementCollection::iterator i = wec.begin();
             i != wec.end() && n < maxNum; ++i, ++n ) {
              if( ( *i ).evaluateJavaScript( cond ).toBool() ) {
                     we.push_back( GetContext()->AddObjToJSContext( new WebElement( *i ) ) );
            }
        }
        return we;
    }
    QList< QVariant > findElements( const QString& selectorQuery ) const {
           QWebElementCollection wec = we_.findAll( selectorQuery );
           if( wec.count() < 1 ) return QList< QVariant >();
           QList< QVariant > we;
           for( QWebElementCollection::const_iterator i = wec.constBegin();
               i != wec.constEnd(); ++i ) {
               we.push_back( GetContext()->AddObjToJSContext( new WebElement( *i ) ) );
           }
           return we;
    }
    void setPlainText( const QString& text ) { we_.setPlainText( text ); }
    QString toPlainText() const { return we_.toPlainText(); }
    void setAttribute( const QString& name, const QString& value ) {
        we_.setAttribute( name, value );
    }
    void setAttributeNS( const QString& namespaceUri, const QString& name, const QString& value ) {
        we_.setAttributeNS( namespaceUri, name, value );
    }
    QString attribute( const QString& name, const QString& defaultValue = QString() ) const {
        return we_.attribute( name, defaultValue );
    }
    QString attributeNS( const QString& namespaceUri, const QString& name, const QString& defaultValue = QString() ) const {
        return we_.attributeNS( namespaceUri, name, defaultValue );
    }
    bool hasAttribute( const QString& name ) const {
        return we_.hasAttribute( name );
    }
    bool hasAttributeNS(const QString& namespaceUri, const QString& name) const {
        return we_.hasAttributeNS( namespaceUri, name );
    }
    void removeAttribute( const QString& name ) { we_.removeAttribute( name ); }
    void removeAttributeNS( const QString& namespaceUri, const QString& name ) {
        we_.removeAttributeNS( namespaceUri, name );
    }
    bool hasAttributes() const { return we_.hasAttributes(); }
    QStringList attributeNames(const QString& namespaceUri = QString()) const {
        return we_.attributeNames( namespaceUri );
    }
    QStringList classes() const { return we_.classes(); }
    bool hasClass( const QString& name ) const { return we_.hasClass( name ); }
    void addClass( const QString& name ) { we_.addClass( name ); }
    void removeClass( const QString& name ) { we_.removeClass( name ); }
    void toggleClass( const QString& name ) { we_.toggleClass( name ); }
    bool hasFocus() const { return we_.hasFocus(); }
    void setFocus() { we_.setFocus(); }
    QVariantMap geometry() const {
        const QRect r = we_.geometry();
        QVariantMap m;
        m[ "x" ] = r.x();
        m[ "y" ] = r.y();
        m[ "width" ] = r.width();
        m[ "height" ] = r.height();
        return m;
    }
    QString tagName() const { return we_.tagName(); }
    QString prefix() const { return we_.prefix(); }
    QString localName() const { return we_.localName(); }
    QString namespaceUri() const { return we_.namespaceUri(); }
    QVariant parent() {
        return GetContext()->AddObjToJSContext( new WebElement( we_.parent() ) );
    }
    QVariant firstChild() {
        return GetContext()->AddObjToJSContext( new WebElement( we_.firstChild() ) );
    }
    QVariant lastChild() {
        return GetContext()->AddObjToJSContext( new WebElement( we_.lastChild() ) );
    }
    QVariant nextSibling() {
        return GetContext()->AddObjToJSContext( new WebElement( we_.nextSibling() ) );
    }
    QVariant previousSibling() {
        return GetContext()->AddObjToJSContext( new WebElement( we_.previousSibling() ) );
    }
    QVariant document() {
        return GetContext()->AddObjToJSContext( new WebElement( we_.document() ) );
    }
    QWebFrame *webFrame() const { return we_.webFrame(); }
    void appendInside( const QString& markup ) { we_.appendInside( markup ); }
    void appendInside( const WebElement& element ) { we_.appendInside( element.qWebElement() ); }
    void prependInside( const QString& markup ) { we_.prependInside( markup ); }
    void prependInside( const WebElement& element ) { we_.prependInside( element.qWebElement() ); }
    void appendOutside( const QString& markup ) { we_.appendOutside( markup ); }
    void appendOutside( const WebElement& element ) { we_.appendInside( element.qWebElement() ); }
    void prependOutside( const QString& markup ) { we_.prependOutside( markup ); }
    void prependOutside( const WebElement& element ) { we_.prependOutside( element.qWebElement() ); }
    void encloseContentsWith( const WebElement& element ) {
        we_.encloseContentsWith( element.qWebElement() );
    }
    void encloseContentsWith( const QString& markup ) {
        we_.encloseContentsWith( markup );
    }
    void encloseWith( const QString& markup ) { we_.encloseWith( markup ); }
    void encloseWith( const WebElement& element ) {
        we_.encloseWith( element.qWebElement() );
    }
    void replace( const QString& markup ) { we_.replace( markup ); }
    void replace( const WebElement& element ) {
           we_.replace( element.qWebElement() );
    }
    QVariant clone() {
        return GetContext()->AddObjToJSContext( new WebElement( we_.clone() ) );
    }
    //QWebElement& takeFromDocument();
    void removeFromDocument() { we_.removeFromDocument(); }
    void removeAllChildren() { we_.removeAllChildren(); }
    QVariant eval( const QString& scriptSource ) {
        return we_.evaluateJavaScript( scriptSource );
    }
private:
    QWebElement we_;
};

}
