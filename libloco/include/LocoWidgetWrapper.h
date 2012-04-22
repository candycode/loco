#pragma once
//#SRCHEADER

#include <QWidget>
#include <QIcon>
#include <QBitmap>
#include <QtGui/QPixmap>
#include <QtGui/QPainter>

#include "LocoWrappedWidget.h"
namespace loco {

//to be able to cast from QObject we need to pass a QObject derived object to qobject_cast 

class  WidgetWrapper : public WrappedWidget {
    Q_OBJECT
    Q_PROPERTY( bool fullScreen READ IsFullScreen )
public:
    WidgetWrapper( QObject* wrappedWidget, //need to use QObject in order to access from Javascript
            Context* c = 0, 
            const QString& n = "LocoWidgetWrapper",
            const QString& type = "", 
            const QString& module = "",
            bool destroyable = false,
            ObjectInfo* objInfo = 0 ) : 
                WrappedWidget( c, n, type, module, destroyable, objInfo ),
                widget_( dynamic_cast< QWidget* >( wrappedWidget ) ) {
                if( widget_ != 0 ) ConnectSignals();

          }
    WidgetWrapper( Context* c = 0, 
            const QString& n = "LocoWrappedWidget",
            const QString& type = "", 
            const QString& module = "",
            bool destroyable = false,
            ObjectInfo* objInfo = 0 ) : 
                WrappedWidget( c, n, type, module, destroyable, objInfo ),
                widget_( 0 ) {}
    bool SetWidget( QObject* ww ) {
        if( !dynamic_cast< QWidget* >( ww ) ) {
            error( "Not a QWidget" );
            return false;
        }
        ///@todo IMPLEMENT
        //if( widget_ != 0 ) DisconnectSignals();
        widget_ = dynamic_cast< QWidget* >( ww );
        connect( widget_, SIGNAL( destroyed() ), this, SIGNAL( destroyed() ) );        
        return true;
    }
public:
    QWidget* Widget() { return widget_; }
    const QWidget* Widget() const { return widget_; }
public slots:
    bool setWidget( QObject* ww ) { return SetWidget( ww ); }
    bool wrap( QObject* ww ) { return SetWidget( ww ); }
    void setLayout( QObject* l ) { SetLayout( l ); }
private:
    QWidget* widget_;
};

}
