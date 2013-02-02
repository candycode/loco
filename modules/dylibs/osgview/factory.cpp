#include "OSGViewPlugin.h"
#include <QVariantMap>

extern "C" QObject* Create( QVariantMap ) {
	return new OSGViewPlugin;
}