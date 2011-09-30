#pragma once
//#SRCHEADER

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

