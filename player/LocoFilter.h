#pragma once
//#SRCHEADER

#include "EWL.h"

#include <QString>

class QPluginLoader;

namespace loco {

class Filter : public EWL {
    Q_OBJECT
public:
    virtual QString Apply( const QString& ) const = 0;
    void SetPluginLoader( QPluginLoader* pl ) { pl_ = pl; }
    QPluginLoader* GetPluginLoader() const { return pl_; }
private:
	QPluginLoader* pl_;
};

}

Q_DECLARE_INTERFACE( loco::Filter, "Loco.Filter:1.0" )
