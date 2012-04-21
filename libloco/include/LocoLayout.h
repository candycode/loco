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
    virtual QString GetLayoutType() const  = 0;
    virtual QObject* GetQLayout() const = 0;
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
    QObject* GetQLayout() const { return layout_; }
public slots:
    void addWidget( QObject* );
private:
    VBoxLayout* layout_;
};


//------------------------------------------------------------------------------
class HBoxLayout : public Layout {
    Q_OBJECT
    Q_PROPERTY( QString layoutType READ GetLayoutType )
public:
    HBoxLayout() : layout_( new HBoxLayout ) {
        connect( this, SIGNAL( destroyed() ), layout_, SLOT( deleteLater() ) );
    }
    QString GetLayoutType() const { return "HBOX"; }            
    QObject* GetQLayout() const { return layout_; }
public slots:
    void addWidget( QObject* );
private:
    HBoxLayout* layout_;
};


}
