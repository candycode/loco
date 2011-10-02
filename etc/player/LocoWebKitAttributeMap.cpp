//#SRCHEADER

#include "LocoWebKitAttributeMap.h"

namespace loco {
    TextAttributeMap WebKitAttributeMap::textToAttributeMap_;
	AttributeTextMap WebKitAttributeMap::attributeToTextMap_;

    WebKitAttributeMap::WebKitAttributeMap() {
    	if( textToAttributeMap_.empty() ) InitMaps();
    }

#ifdef LOCO_MAPATTR
#error "LOCO_MAPATTR ALREADY DEFINED!"
#endif
#define LOCO_MAPATTR(a) { \
	am[ #a ] = QWebSettings:: a ; \
    ma[ QWebSettings:: a ] = #a ; \
    }

    void WebKitAttributeMap::InitMaps() {
    	TextAttributeMap& am = textToAttributeMap_;
    	AttributeTextMap& ma = attributeToTextMap_;
    	LOCO_MAPATTR( AutoLoadImages );
    	LOCO_MAPATTR( DnsPrefetchEnabled );
    	LOCO_MAPATTR( JavascriptEnabled );
    	LOCO_MAPATTR( JavaEnabled );
    	LOCO_MAPATTR( PluginsEnabled );
    	LOCO_MAPATTR( PrivateBrowsingEnabled );
    	LOCO_MAPATTR( JavascriptCanOpenWindows );
    	LOCO_MAPATTR( DeveloperExtrasEnabled );
    	LOCO_MAPATTR( SpatialNavigationEnabled );
    	LOCO_MAPATTR( ZoomTextOnly );
    	LOCO_MAPATTR( PrintElementBackgrounds );
    	LOCO_MAPATTR( OfflineStorageDatabaseEnabled );
    	LOCO_MAPATTR( OfflineWebApplicationCacheEnabled );
    	LOCO_MAPATTR( LocalStorageEnabled );
    	LOCO_MAPATTR( LocalStorageDatabaseEnabled );
    	LOCO_MAPATTR( LocalContentCanAccessRemoteUrls );
    	LOCO_MAPATTR( LocalContentCanAccessFileUrls );
    	LOCO_MAPATTR( XSSAuditingEnabled );
    	LOCO_MAPATTR( AcceleratedCompositingEnabled );
    	LOCO_MAPATTR( TiledBackingStoreEnabled );
    	LOCO_MAPATTR( FrameFlatteningEnabled );
    	LOCO_MAPATTR( SiteSpecificQuirksEnabled );
#ifdef LOCO_WEBGL
   	    LOCO_MAPATTR( WebGLEnabled );
#endif
    }
#undef LOCO_MAPATTR
}

