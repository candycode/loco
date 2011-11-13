#pragma once
//#SRCHEADER
#include <limits>

#include <QtGui/QMainWindow>
#include <QString>
#include <QVariantMap>
#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <QSignalMapper>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QMap>
#include <QtGui/QPixmap>
#include <QtGui/QBitmap>
#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebSettings>
#include <QtGui/QToolBar>
#include <QVariantList>
#include <QAbstractNetworkCache>

#include "LocoObject.h"
#include "LocoContext.h"
#include "LocoWebView.h"
#include "LocoWebKitAttributeMap.h"
#include "LocoWebKitJSCoreWrapper.h"
#include "LocoDynamicWebPluginFactory.h"
#include "LocoStaticWebPluginFactory.h"
#include "LocoWebElement.h"

namespace loco {

typedef QMap< QString, QMenu* > MenuMap;
typedef QMap< QString, QAction* > ActionMap;
typedef QMap< QAction*, QString > ActionPath;
typedef QMap< QAction*, QString > CBackMap;


class MainWindow : public Object {
    Q_OBJECT
public:
    MainWindow() : Object( 0, "LocoMainWindow", "Loco/GUI/Window" ), toolBar_( 0 )  {
      
        mapper_ = new QSignalMapper( this );
        connect( mapper_, SIGNAL( mapped( QObject* ) ), this, SLOT( ActionTriggered( QObject* ) ) );
    }
    QMainWindow* GetMainWindow() { return &mw_; }

public slots:
	bool setCentralWidget( QObject* widget ) {
	    QWidget* w = qobject_cast< QWidget* >( widget );
		if( !w ) {
			error( "'setCentralWidget' requires a QWidget instance" );
			return false;
		}
		mw_.setCentralWidget( w );
		return true;
	}
	//QObject* getCentralWidget() { return mw_.centralWidget(); }
    bool fullScreen() const { return mw_.isFullScreen(); }
    void toggleFullScreen() {
    	if( mw_.isFullScreen() ) mw_.showNormal();
    	else mw_.showFullScreen();
    }
    void close() { mw_.close(); }
    void setWindowOpacity( double op ) { mw_.setWindowOpacity( op ); }
    double windowOpacity() const { return mw_.windowOpacity(); }
    void setMask( const QPixmap& p ) {
        mw_.setMask( p.mask() );
    }
    void setMask( const QString& ppath ) {
        QPixmap p( ppath );
        mw_.setMask( p.mask() );
    }  
    void setStatusBarText( const QString& text, int timeout = 0 ) {
        mw_.statusBar()->showMessage( text, timeout );
    }
    void setMenu( const QVariantMap& jsonTree ) { SetMenu( 0, jsonTree, "" ); }
    void setMenuItemProperty( const QString& path, const QString& prop, const QString& value ) {
        MenuMap::iterator mi = menuItems_.find( path );
        if( mi != menuItems_.end() ) {
            SetMenuProperty( mi.value(), prop, value );
        } else {
            ActionMap::iterator ai = actions_.find( path );
            if( ai != actions_.end() ) {
                SetActionProperty( ai.value(), prop, value );
            }
            else error( "Menu item " + path + " not found" );
        }
    }
    QString getMenuItemProperty( const QString& path, const QString& prop ) const {
        MenuMap::const_iterator mi = menuItems_.find( path );
        if( mi != menuItems_.end() ) {
            return GetMenuProperty( mi.value(), prop );
        } else {
            ActionMap::const_iterator ai = actions_.find( path );
            if( ai != actions_.end() ) {
                return GetActionProperty( ai.value(), prop );
            }
            else {
                error( "Menu item " + path + " not found" );
                return "";
            }
        }
    }
private slots:
    void ActionTriggered( QObject* action ) {
        QAction* a = qobject_cast< QAction* >( action );
        if( !a ) error( "Type mismatch" );
        else {
            CBackMap::const_iterator i = cbacks_.find( a );
            if( i != cbacks_.end() ) GetContext()->Eval( cbacks_[ a ] );
            else emit actionTriggered( actionPath_[ a ] );
        }
    }
public slots:
    void setMouseCursor( const QString& shape ) {
        if( shape == "arrow" ) mw_.setCursor( Qt::ArrowCursor );
        else if( shape == "upArrow" ) mw_.setCursor( Qt::UpArrowCursor );
        else if( shape == "cross" )  mw_.setCursor( Qt::CrossCursor );
        else if( shape == "wait" ) mw_.setCursor( Qt::WaitCursor );
        else if( shape == "ibeam" ) mw_.setCursor( Qt::IBeamCursor );
        else if( shape == "sizeVert" ) mw_.setCursor( Qt::SizeVerCursor );
        else if( shape == "sizeHor" ) mw_.setCursor( Qt::SizeHorCursor );
        else if( shape == "wait" ) mw_.setCursor( Qt::WaitCursor );
        else if( shape == "sizeAll" ) mw_.setCursor( Qt::SizeAllCursor );
        else if( shape == "hide" ) mw_.setCursor( Qt::BlankCursor );
        else if( shape == "splitVert" ) mw_.setCursor( Qt::SplitVCursor );
        else if( shape == "splitHor" ) mw_.setCursor( Qt::SplitHCursor );
        else if( shape == "pointHand" ) mw_.setCursor( Qt::PointingHandCursor );
        else if( shape == "forbid" ) mw_.setCursor( Qt::ForbiddenCursor );
        else if( shape == "openHand" ) mw_.setCursor( Qt::OpenHandCursor );
        else if( shape == "closedHand" ) mw_.setCursor( Qt::ClosedHandCursor );
        else if( shape == "whatsThis" ) mw_.setCursor( Qt::WhatsThisCursor );
        else if( shape == "busy" ) mw_.setCursor( Qt::BusyCursor );
        else if( shape == "dragMove" ) mw_.setCursor( Qt::DragMoveCursor );
        else if( shape == "dragCopy" ) mw_.setCursor( Qt::DragCopyCursor );
        else if( shape == "dragLink" ) mw_.setCursor( Qt::DragLinkCursor ); 
    }
    void setMouseCursor( const QPixmap& pmap, int hotX = -1, int hotY = -1 ) {
        QCursor c( pmap, hotX, hotY );
        mw_.setCursor( c );
    }
    void setToolbar( const QVariantList& jsonData ) {
       if( toolBar_ == 0 ) {
           toolBar_ = new QToolBar;
           mw_.addToolBar( toolBar_ );
       }  
       for( QVariantList::const_iterator i = jsonData.begin(); i != jsonData.end(); ++i ) {
            if( !i->toMap().isEmpty() ) {
                const QVariantMap m = i->toMap();
                if( m[ "type" ].toString() == "separator" ) {
                    toolBar_->addSeparator();
                } else if(  m[ "type"  ].toString() == "button" &&
                           !m[ "icon"  ].toString().isEmpty()   &&
                           !m[ "path"  ].toString().isEmpty()   &&
                           !m[ "cback" ].toString().isEmpty() ) {
                    QAction* a = new QAction( QIcon( m[ "icon" ].toString() ), m[ "text" ].toString(), toolBar_ );
                    a->setToolTip( m[ "tooltip" ].toString() );
                    a->setStatusTip( m[ "status" ].toString() );
                    mapper_->setMapping( a, a );
                    connect( a, SIGNAL( triggered() ), mapper_, SLOT( map() ) );
                    cbacks_[ a ] = m[ "cback" ].toString(); 
                    actions_[ "path" ] = a;
                    actionPath_[ a ] = "path";     
                }
            }            
        }
    }
    void hideToolBar() { toolBar_->hide(); }
    void showToolBar() { toolBar_->show(); }
    void hideMenu() { mw_.menuBar()->hide(); }
    void showMenu() { mw_.menuBar()->show(); }
    void hideStatusBar() { mw_.statusBar()->hide(); }
    void showStatusBar() { mw_.statusBar()->show(); }
    void setWindowIcon( const QPixmap& p ) { mw_.setWindowIcon( p ); }
    void setWindowIcon( const QString& f ) { mw_.setWindowIcon( QIcon( f ) ); }  
    void move( int x, int y ) { mw_.move( x, y ); }
    void setName( const QString& n ) { Object::setName( n ); }
    void setMenuBarVisibility( bool on ) { if( mw_.menuBar() ) mw_.menuBar()->setVisible( on ); }
    void setStatusBarVisibility( bool on ) { if( mw_.statusBar() ) mw_.statusBar()->setVisible( on ); }
    void setWindowTitle( const QString& t ) { mw_.setWindowTitle( t ); }
    QString title() const { return mw_.windowTitle(); }
    void showNormal() { mw_.showNormal(); }
    void showFullScreen() { mw_.showFullScreen(); }
    void show() {  mw_.show(); }
    //QVariant geometry() const;
    //void setGeometry( const QVariant& g );

private:
   void SetMenu( QMenu* parent, const QVariantMap& jsonTree, QString path ) {
         for( QVariantMap::const_iterator i = jsonTree.begin();
              i != jsonTree.end(); ++i ) {
             const bool leaf = i.value().type() == QVariant::Map &&
                               (i.value().toMap().begin()).value().type() == QVariant::String;
             if( leaf ) {
                 QString key =  i.key();
                 key.remove( 0, key.indexOf(  QRegExp("[^0-9_]") ) ); //prepend with _<number>_ to force ordering
                 QAction* a = new QAction( key, &mw_ );
                 mapper_->setMapping( a, a );
                 connect( a, SIGNAL( triggered() ), mapper_, SLOT( map() ) );
                 parent->addAction( a );
                 QVariantMap m = i.value().toMap();
                 a->setToolTip( m[ "tooltip" ].toString() );
                 a->setStatusTip( m[ "status" ].toString() );
                 a->setIcon( QIcon( m[ "icon" ].toString() ) );
                 a->setIconText( m[ "icon_text" ].toString() ); 
                 QString cback = m[ "cback" ].toString();
                 if( !cback.isEmpty() ) cbacks_[ a ] = cback;
                 actions_[ path ] = a;
                 actionPath_[ a ] = path;
             } else {
                 QMenu* m = 0;
                 if( parent != 0 ) {
                   const QString entry = i.key();
                   if( !entry.startsWith( "__" ) && !entry.startsWith( "--" ) ) {   
                         m = parent->addMenu( i.key() );
                   } else {
                       parent->addSeparator();
                       continue;
                   }                   
                 } else {
                	 QString key =  i.key();
                	 key.remove( 0, key.indexOf(  QRegExp("[^0-9_]") ) ); //prepend with _<number>_ to force ordering
                     m = mw_.menuBar()->addMenu( key );
                 }
                 const QString menuPath = path + "/" + i.key();
                 menuItems_[ menuPath ] = m;
                 SetMenu( m, i.value().toMap(), menuPath );
             }
         }
   }
   void SetMenuProperty( QMenu*, const QString&, const QString& ) {}
   void SetActionProperty( QAction*, const QString&, const QString& ) {}
   QString GetMenuProperty( QMenu*, const QString& ) const { return ""; }
   QString GetActionProperty( QAction*, const QString& ) const { return ""; }

signals:
    void actionTriggered( const QString& path );
    void closing();
private:
    QMainWindow mw_;
    MenuMap menuItems_;
    ActionMap actions_;
    CBackMap cbacks_;
    ActionPath actionPath_;
    QSignalMapper* mapper_;
    QToolBar* toolBar_;    
};

}
