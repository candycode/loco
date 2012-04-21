#pragma once
//#SRCHEADER

#include <QWidget>
#include <QString>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include "LocoObject.h"


namespace loco {

class Layout : public Object {
    Q_OBJECT
public:
    Layout( Context* c, 
            const QString& n,
            const QString& type, 
            const QString& module,
            bool destroyable,
            ObjectInfo* objInfo ) : Object( c, n, type, module, destroyable, objInfo ) {}
    virtual QObject* GetQLayout() const = 0;
    virtual QString GetLayoutType() const = 0;
    
};

class VBoxLayout : public Layout {
    Q_OBJECT
    Q_PROPERTY( QString layoutType READ GetLayoutType )
public:
    VBoxLayout( Context* c = 0, 
            const QString& n = "LocoVBoxLayout",
            const QString& type = "", 
            const QString& module = "",
            bool destroyable = false,
            ObjectInfo* objInfo = 0 ) : Layout( c, n, type, module, destroyable, objInfo ),
                                        layout_( new QVBoxLayout ) { 
        connect( this, SIGNAL( destroyed() ), layout_, SLOT( deleteLater() ) );
    }
    QString GetLayoutType() const { return "VBOX"; }            
    QObject* GetQLayout() const { return layout_; }
public slots:
    void addWidget( QObject* );
private:
    QVBoxLayout* layout_;
};


//------------------------------------------------------------------------------
class HBoxLayout : public Layout {
    Q_OBJECT
    Q_PROPERTY( QString layoutType READ GetLayoutType )
public:
    HBoxLayout( Context* c = 0, 
            const QString& n = "LocoHBoxLayout",
            const QString& type = "", 
            const QString& module = "",
            bool destroyable = false,
            ObjectInfo* objInfo = 0 ) : Layout( c, n, type, module, destroyable, objInfo ),
                                        layout_( new QHBoxLayout ) {  
        connect( this, SIGNAL( destroyed() ), layout_, SLOT( deleteLater() ) );
    }
    QString GetLayoutType() const { return "HBOX"; }            
    QObject* GetQLayout() const { return layout_; }
public slots:
    void addWidget( QObject* );
private:
    QHBoxLayout* layout_;
};


}
