#pragma once
//#SRCHEADER

#include <QWidget>
#include <QString>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include "LocoObject.h"


namespace loco {

class Layout : Object {
    Q_OBJECT
//public:
//    virtual QString GetLayoutType() const  = 0;
//    virtual QLayout* GetQLayout() const = 0;
public slots:
    void addWidget( QObject* );
};

//------------------------------------------------------------------------------
class VBoxLayout : public Layout {
    Q_OBJECT
    Q_PROPERTY( QString layoutType READ GetLayoutType )
public:
    VBoxLayout() : layout_( new VBoxLayout ) {
        connect( this, SIGNAL( destroyed() ), layout_, SLOT( deleteLater() ) );
    }
    QString GetLayoutType() const { return "VBOX"; }            
    QLayout* GetQLayout() const { return layout_; }
private:
    VBoxLayout* layout_;
};


//------------------------------------------------------------------------------
class HBoxLayout : public Object {
    Q_OBJECT
    Q_PROPERTY( QString layoutType READ GetLayoutType )
public:
    HBoxLayout() : layout_( new HBoxLayout ) {
        connect( this, SIGNAL( destroyed() ), layout_, SLOT( deleteLater() ) );
    }
    QString GetLayoutType() const { return "HBOX"; }            
    QLayout* GetQLayout() const { return layout_; }
private:
    HBoxLayout* layout_;
};


}
