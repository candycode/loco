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


#include <stdexcept>
#include <string>

#include <QtWebKit/QWebSettings>
#include <QString>
#include <QMap>


namespace loco {

typedef QMap< QString, QWebSettings::WebAttribute > TextAttributeMap;
typedef QMap< QWebSettings::WebAttribute, QString > AttributeTextMap;

class WebKitAttributeMap {
public:
    WebKitAttributeMap();
    QWebSettings::WebAttribute operator[]( const QString& sattr ) const {
        TextAttributeMap::const_iterator i = textToAttributeMap_.find( sattr );
        if( i == textToAttributeMap_.end() )
            throw( std::runtime_error( "Attribute " + sattr.toStdString() + " not found" ) );
        else {
            return i.value();
        }
    }
    const QString& operator[]( QWebSettings::WebAttribute attr ) const {
        AttributeTextMap::const_iterator i = attributeToTextMap_.find( attr );
        if( i == attributeToTextMap_.end() )
            throw( std::runtime_error( "Attribute not found" ) );
        else {
            return i.value();
        }
    }
    const AttributeTextMap& GetAttributeToTextMap() const { return attributeToTextMap_; }
private:
    static void InitMaps();
private:
    static TextAttributeMap textToAttributeMap_;
    static AttributeTextMap attributeToTextMap_;
};

}

