#pragma once
//#SRCHEADER
#include <QMainWindow>
#include <QString>
#include <QVariantMap>
#include <QMenu>
#include <QAction>
#include <QSignalMapper>
#include <QMenuBar>
#include <QStatusBar>
#include <QMap>

#include "LocoObject.h"
#include "LocoContext.h"
#include "LocoWebWindow.h"

namespace loco {

typedef QMap< QString, QMenu* > MenuMap;
typedef QMap< QString, QAction* > ActionMap;
typedef QMap< QAction*, QString > ActionPath;
typedef QMap< QAction*, QString > CBackMap;


class WebMainWindow : public WebWindow {
    Q_OBJECT
public:
    WebMainWindow() {
	    SetName( "LocoWebMainWindow" );
	    SetType( "Loco/GUI/Window"   );
	    mw_.setCentralWidget( GetWebView() );
    }
public slots:
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

    virtual void setWindowTitle( const QString& t ) { mw_.setWindowTitle( t ); }
    virtual QString title() const { return mw_.windowTitle(); }
    virtual void showNormal() { mw_.showNormal(); }
    virtual void showFullScreen() { mw_.showFullScreen(); }
    virtual void show() {  mw_.show(); }


signals:
    void actionTriggered( const QString& path );

private:
   void SetMenu( QMenu* parent, const QVariantMap& jsonTree, QString path ) {
  	   for( QVariantMap::const_iterator i = jsonTree.begin();
  			i != jsonTree.end(); ++i ) {
  		   const bool leaf = i.value().type() == QVariant::Map &&
  				             (i.value().toMap().begin()).value().type() == QVariant::String;
  		   if( leaf ) {
  			   QAction* a = new QAction( i.key(), &mw_ );
  			   mapper_.setMapping( a, a );
  		       connect( a, SIGNAL( triggered(bool) ), &mapper_, SLOT( map() ) );
  			   parent->addAction( a );
  			   QString cback = i.value().toMap()[ "cback" ].toString();
  			   printf( "%s\n", cback.toStdString().c_str());
  		       if( !cback.isEmpty() ) cbacks_[ a ] = cback;
  		       actions_[ path ] = a;
  		       actionPath_[ a ] = path;
  		   }
  		   else {
  			   QMenu* m = 0;
  		       if( parent != 0 ) {

  			       m = parent->addMenu( i.key() );
  		       } else {
  		    	   m = mw_.menuBar()->addMenu( i.key() );
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

private slots:
    void ActionTriggered( QObject* action ) {
        QAction* a = qobject_cast< QAction* >( action );
        printf("!!!!\n");
        if( !a ) error( "Type mismatch" );
        else {
        	CBackMap::const_iterator i = cbacks_.find( a );
            if( i != cbacks_.end() ) Eval( cbacks_[ a ] );
            else emit actionTriggered( actionPath_[ a ] );
        }
    }

private:
   QMainWindow mw_;
   MenuMap menuItems_;
   ActionMap actions_;
   CBackMap cbacks_;
   ActionPath actionPath_;
   QSignalMapper mapper_;
};

}
