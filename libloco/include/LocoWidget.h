#pragma once
//#SRCHEADER

#include <QWidget>
#include <QIcon>
#include <QBitmap>
#include <QCursor>
#include <QIcon>
#include <QtGui/QPixmap>
#include <QtGui/QPainter>

#include "LocoObject.h"
#include "LocoContext.h"
#include "LocoLayout.h"

namespace loco {

//to be able to cast from QObject we need to pass a QObject derived object to qobject_cast 

class  Widget : public Object {
    Q_OBJECT
    Q_PROPERTY( bool fullScreen READ IsFullScreen )
    Q_PROPERTY( QObject* parentWidget READ GetParentWidget WRITE SetParentWidget )
    Q_PROPERTY( QObject* qwidget READ GetQWidget )
    Q_PROPERTY( QObject* layout READ GetLayout WRITE SetLayout ) 
public:
    Widget( Context* c = 0, 
            const QString& n = "LocoWidget",
            const QString& type = "", 
            const QString& module = "",
            bool destroyable = false,
            ObjectInfo* objInfo = 0 ) : Object( c, n, type, module, destroyable, objInfo ), 
                                        widget_( new QWidget ), layout_( 0 ) {
        connect( this, SIGNAL( destroyed() ), widget_, SLOT( deleteLater() ) );
    }
    bool IsFullScreen() const { return widget_->isFullScreen(); }
    virtual void SetParentWidget( QObject* pw) {
        if( qobject_cast< QWidget* >( pw ) ) {
            widget_->setParent( qobject_cast< QWidget* >( pw ) );
        } else if( qobject_cast< Widget* >( pw ) ) {
            QWidget* w =  qobject_cast< QWidget* >(
                              qobject_cast< Widget* >( pw )->GetQWidget() );
            widget_->setParent( w );   
        } else error( "Widget::parentWidget must bw a Widget or QWidget type only" );
    }
    QObject* GetParentWidget() const { return parent(); }
    QObject* GetQWidget() const { return widget_; }
    QObject* GetLayout() const { return layout_; }
    void SetLayout( QObject* l ) {
        if( !qobject_cast< Layout* >( l ) ) {
            error( "Widget::layout must be a Layout type" );
        } else {
            if( layout_ ) layout_->deleteLater();
            layout_ = qobject_cast< Layout* >( l );
            layout_->setParent( this );  
            QLayout* l = qobject_cast< QLayout* >( layout_->GetQLayout() );
            widget_->setLayout( l );
            layout_->setParent( this ); 
        } 
    }
public slots:
    void setLayout( QObject* l ) { SetLayout( l ); }
    void resize( int w, int h ) { widget_->resize( w, h ); }
    void close() { widget_->close(); }
    /// @todo void setMouseCursor( const QString& shape );
    void setMouseCursor( const QPixmap& pmap, int hotX = -1, int hotY = -1 ) {
        QCursor c( pmap, hotX, hotY );
        widget_->setCursor( c );
    }
    void setWindowAttribute( Qt::WidgetAttribute wa ) {
        widget_->setAttribute( wa );
    }
    void setWindowIcon( const QPixmap& p ) { widget_->setWindowIcon( p ); }
    void setWindowIcon( const QString& f ) { widget_->setWindowIcon( QIcon( f ) ); }
    void move( int x, int y ) { widget_->move( x, y ); }
    ///@todo void setWindowStyle( const QStringList& flags );
    void setWindowOpacity( double op ) { widget_->setWindowOpacity( op ); }
    double windowOpacity() const { return widget_->windowOpacity(); }
    void setMask( const QPixmap& p ) {
        widget_->setMask( p.mask() );
    }
    void setMask( const QString& ppath ) {
        QPixmap p( ppath );
        widget_->setMask( p.mask() );
    }
    void setWindowTitle( const QString& t ) { widget_->setWindowTitle( t ); }
    QString title() const { return widget_->windowTitle(); }
    void showNormal() { widget_->showNormal(); }
    void showFullScreen() { widget_->showFullScreen(); }
    void show() {  widget_->show(); }
    void show( int w, int h ) {  widget_->show(); widget_->resize( w, h ); }
    void hide() {  widget_->hide(); }
signals:
    void closing();
private:
    QWidget* widget_;
    Layout* layout_;   
};

}
