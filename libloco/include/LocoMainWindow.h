#pragma once
////////////////////////////////////////////////////////////////////////////////
//Copyright (c) 2012, Ugo Varetto
//All rights reserved.
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions are met:
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder nor the
//      names of its contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//DISCLAIMED. IN NO EVENT SHALL UGO VARETTO BE LIABLE FOR ANY
//DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////

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
#include <QtGui/QToolBar>
#include <QVariantList>

#include "LocoObject.h"
#include "LocoContext.h"

#include "LocoWrappedWidget.h"
namespace loco {

typedef QMap< QString, QMenu* > MenuMap;
typedef QMap< QString, QAction* > ActionMap;
typedef QMap< QAction*, QString > ActionPath;
typedef QMap< QAction*, QString > CBackMap;


class MainWindow : public WrappedWidget {
    Q_OBJECT
public:
    MainWindow() : WrappedWidget( 0, "LocoMainWindow", "Loco/GUI/Window" ), toolBar_( 0 )  {
        mapper_ = new QSignalMapper( this );
        connect( mapper_, SIGNAL( mapped( QObject* ) ), this, SLOT( ActionTriggered( QObject* ) ) );
    }
    ///@warning returned pointer shall not be used for embedding main window into other objects
    QMainWindow* GetMainWindow() { return &mw_; }
    virtual QWidget* Widget() { return &mw_; }
    virtual const QWidget* Widget() const { return &mw_; }
    virtual void SetParentWidget( WrappedWidget* pw) {
        throw std::logic_error( "'MainWindow' is a top level Widget not to embed into anything else!" );
    }
public slots:
	bool setCentralWidget( QObject* widget ) {
		QWidget *w = qobject_cast< QWidget* >( widget );
		if( !w ) {
	        WrappedWidget* ww = dynamic_cast< WrappedWidget* >( widget );
			if( !ww ) {
			   error( "'setCentralWidget' requires a QWidget or WrappedWidget instance" );
			   return false;
			} else {
				w = ww->Widget();
			}
		}
		mw_.setCentralWidget( w );
		return true;
	}
    void setStatusBarText( const QString& text, int timeout = 0 ) {
        mw_.statusBar()->showMessage( text, timeout );
    }
    void setMenu( const QVariantMap& jsonTree ) { SetMenu( 0, jsonTree, "" ); }
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
                    actions_[ m[ "path" ].toString() ] = a;
                    actionPath_[ a ] = m[ "path" ].toString();
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
    void setMenuBarVisibility( bool on ) { if( mw_.menuBar() ) mw_.menuBar()->setVisible( on ); }
    void setStatusBarVisibility( bool on ) { if( mw_.statusBar() ) mw_.statusBar()->setVisible( on ); }
    bool setActionProperties( const QString& actionPath, const QVariantMap& p ) {
    	if( !actions_.contains( actionPath ) ) {
    	    error( "Action path " + actionPath + " not found" );
    	    return false;
    	} else {
    	    ConfigureAction( actions_[ actionPath ], p );
    	    return false;
    	}
    }

private:
    void ConfigureAction( QAction* a, const QVariantMap& m ) {
		a->setToolTip( m[ "tooltip" ].toString() );
		a->setStatusTip( m[ "status" ].toString() );
		a->setIcon( QIcon( m[ "icon" ].toString() ) );
		a->setIconText( m[ "icon_text" ].toString() );
		a->setChecked( m[ "checked" ].toBool() );
		QString cback = m[ "cback" ].toString();
		if( !cback.isEmpty() ) cbacks_[ a ] = cback;
    }
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
			    ConfigureAction( a, i.value().toMap() );
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
signals:
    void actionTriggered( const QString& path );
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
