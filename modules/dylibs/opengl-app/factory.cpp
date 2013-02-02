#include "GLApp.h"
#include <QVariantMap>

extern "C" QObject* Create( QVariantMap ) {
	return new GLApp;
}
