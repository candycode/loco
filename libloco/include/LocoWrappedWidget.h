#pragma once
//#SRCHEADER

#include <QWidget>
#include <QIcon>
#include <QBitmap>
#include <QtGui/QPixmap>
#include <QtGui/QPainter>

#include "LocoObject.h"
#include "LocoLayout.h"

namespace loco {

//to be able to cast from QObject we need to pass a QObject derived object to qobject_cast 

class  WrappedWidget : public Object {
    Q_OBJECT
    Q_PROPERTY( bool fullScreen READ IsFullScreen )
    Q_PROPERTY( QObject* qwidget READ GetWidget() )
public:
    WrappedWidget( Context* c = 0, 
                   const QString& n = "LocoWrappedWidget",
                   const QString& type = "", 
                   const QString& module = "",
                   bool destroyable = false,
                   ObjectInfo* objInfo = 0 ) : Object( c, n, type, module, destroyable, objInfo ) {}

    bool IsFullScreen() const { return Widget()->isFullScreen(); }
    QObject* GetWidget() { return Widget(); }
public:
    virtual QWidget* Widget()  = 0;
    virtual const QWidget* Widget() const = 0;
    // ownership of wrapped widget transferred to widget wrapped by parent WrappedWidget
    virtual void SetParentWidget( WrappedWidget* pw) {
        Widget()->setParent( pw->Widget() );
    }
    void SetLayout( QObject* ly ) {
        Layout* l = qobject_cast< Layout* >( ly );
        if( !l ) {
            error( "WrappedWidget::setLayout: Layout type required" );
            return;
        } else {
            Widget()->setLayout( qobject_cast< QLayout* >( l->GetQLayout() ) );
        } 
    }
    virtual ~WrappedWidget() {}
protected:
    // problem: not possible to call this method from within constructor method
    // because derived instance is created after base (this) instance and
    // Widget() method might return a pointer to uninitialized data
    // solution: call thid method explicitly from within derived class constructor
    void ConnectSignals() {
        connect( Widget(), SIGNAL( closing() ), this, SIGNAL( closing() ) );
    }
public slots:
    void setStyleSheet( const QString& s ) { 
        Widget()->setStyleSheet( s );
    }
    void resize( int w, int h ) { Widget()->resize( w, h ); }
    bool setParentWidget( QObject* obj ) {
        if( !dynamic_cast< WrappedWidget* >( obj ) ) {
            error( "'setParentWidget' requires a WrappedWidget derived object" );
            return false;
        } else {
            SetParentWidget( dynamic_cast< WrappedWidget* >( obj ) );
            return true;
        }
    }
    bool setParentWindow( QObject* obj ) { return setParentWidget( obj ); }
    void close() { Widget()->close(); }
    void setMouseCursor( const QString& shape );
    void setMouseCursor( const QPixmap& pmap, int hotX = -1, int hotY = -1 ) {
        QCursor c( pmap, hotX, hotY );
        Widget()->setCursor( c );
    }
    void setWindowAttribute( Qt::WidgetAttribute wa ) {
        Widget()->setAttribute( wa );
    }
    void setWindowIcon( const QPixmap& p ) { Widget()->setWindowIcon( p ); }
    void setWindowIcon( const QString& f ) { Widget()->setWindowIcon( QIcon( f ) ); }
    void move( int x, int y ) { Widget()->move( x, y ); }
    void setWindowStyle( const QStringList& flags );
    void setWindowOpacity( double op ) { Widget()->setWindowOpacity( op ); }
    double windowOpacity() const { return Widget()->windowOpacity(); }
    void setMask( const QPixmap& p ) {
        Widget()->setMask( p.mask() );
    }
    void setMask( const QString& ppath ) {
        QPixmap p( ppath );
        Widget()->setMask( p.mask() );
    }
    void setWindowTitle( const QString& t ) { Widget()->setWindowTitle( t ); }
    QString title() const { return Widget()->windowTitle(); }
    void showNormal() { Widget()->showNormal(); }
    void showFullScreen() { Widget()->showFullScreen(); }
    void show() {  Widget()->show(); }
    void show( int w, int h ) {  Widget()->show(); Widget()->resize( w, h ); }
    void hide() {  Widget()->hide(); }
signals:
    void closing();
};

}
