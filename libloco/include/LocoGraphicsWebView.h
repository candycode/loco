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


#include <stdexcept>

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QtWebKit/QGraphicsWebView>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPointer>
#include <QAction>
#include <QtWebKit/QWebFrame>
#include <QtOpenGL/QGLWidget>

class QWebPluginFactory;

class GraphicsWebView : public QGraphicsView {
	Q_OBJECT
public:
	GraphicsWebView() : gwebView_( new QGraphicsWebView ), 
                        eatContextMenuEvent_( false ),
                        eatKeyEvents_( false ),
                        eatMouseEvents_( false ) {
        setFrameShape(QFrame::NoFrame);
		QGraphicsScene* gs = new QGraphicsScene;
        gs->addItem( gwebView_.data() );
        setScene( gs );

    }
	void EatContextMenuEvent( bool yes ) { eatContextMenuEvent_ = yes; }
	bool EatingContextMenuEvent() const { return eatContextMenuEvent_; }
	void EatKeyEvents( bool yes ) { eatKeyEvents_ = yes; }
	bool EatingKeyEvents() const { return eatKeyEvents_; }
    void EatMouseEvents( bool yes ) { eatMouseEvents_ = yes; }
    bool EatingMouseEvents() const { return eatMouseEvents_; }
    void SetWebPluginFactory( QWebPluginFactory* pf ) {
        gwebView_->page()->setPluginFactory( pf );       
    }
    QWebPluginFactory* GetWebPluginFactory() const { return gwebView_->page()->pluginFactory(); }
    const QWebPage* page() const { return gwebView_->page(); }
    QWebPage* page() { return gwebView_->page(); }
    QAction* pageAction( QWebPage::WebAction action ) { return gwebView_->pageAction( action ); }
    void load( const QString& url ) { gwebView_->load( url ); }
    void setHtml( const QString& html, const QUrl& baseUrl ) { 
        gwebView_->setHtml( html, baseUrl );
    }
    void setZoomFactor( qreal zf ) { gwebView_->setZoomFactor( zf ); }
    qreal zoomFactor() const { return gwebView_->zoomFactor(); }
    void setTextSizeMultiplier( qreal tm ) { gwebView_->page()->mainFrame()->setTextSizeMultiplier( tm ); }
    qreal textSizeMultiplier() const { return gwebView_->page()->mainFrame()->textSizeMultiplier(); }
    void reload() { gwebView_->reload(); }
    void setContentEditable( bool yes ) { gwebView_->page()->setContentEditable( yes ); }
    qint64 totalBytes() const { return gwebView_->page()->totalBytes(); }
    bool isModified() const { return false; }
    void triggerPageAction( QWebPage::WebAction action, bool checked ) {
    	throw std::logic_error( "'triggerPageAction' not implemented" );
    }
    
protected:
	void closeEvent( QCloseEvent* e ) {
		emit closing();
		QGraphicsView::closeEvent( e );
	}
	void contextMenuEvent( QContextMenuEvent* e ) {
		if( !eatContextMenuEvent_ ) QGraphicsView::contextMenuEvent( e );
	}
	void keyPressEvent( QKeyEvent* ke ) {
		emit keyPress( ke->key(), ke->modifiers(), ke->count() );
		if( !eatKeyEvents_ ) QGraphicsView::keyPressEvent( ke );
	}
	void keyReleaseEvent( QKeyEvent* ke ) {
		emit keyPress( ke->key(), ke->modifiers(), ke->count() );
		if( !eatKeyEvents_ ) QGraphicsView::keyReleaseEvent( ke );
	}
    void mouseMoveEvent( QMouseEvent* me ) {
        emit mouseMove( me->globalX(),
                        me->globalY(),
                        me->x(),
                        me->y(),
                        bool( me->buttons() & Qt::LeftButton ),
                        bool( me->buttons() & Qt::MiddleButton ),
                        bool( me->buttons() & Qt::RightButton ) );
       if( !eatMouseEvents_ ) QGraphicsView::mouseMoveEvent( me );  
    }
    void mousePressEvent( QMouseEvent* me ) {
        emit mousePress( me->globalX(),
                         me->globalY(),
                         me->x(),
                         me->y(),
                         bool( me->buttons() & Qt::LeftButton ),
                         bool( me->buttons() & Qt::MiddleButton ),
                         bool( me->buttons() & Qt::RightButton ) );
        if( !eatMouseEvents_ ) QGraphicsView::mousePressEvent( me );  
    }
    void mouseReleaseEvent( QMouseEvent* me ) {
        emit mouseRelease( me->globalX(),
                           me->globalY(),
                           me->x(),
                           me->y(),
                           bool( me->buttons() & Qt::LeftButton ),
                           bool( me->buttons() & Qt::MiddleButton ),
                           bool( me->buttons() & Qt::RightButton ) );
       if( !eatMouseEvents_ ) QGraphicsView::mouseReleaseEvent( me );  
    }
    void mouseDoubleClickEvent( QMouseEvent* me ) {
        emit mouseDoubleClick( me->globalX(),
                               me->globalY(),
                               me->x(),
                               me->y(),
                               bool( me->buttons() & Qt::LeftButton ),
                               bool( me->buttons() & Qt::MiddleButton ),
                               bool( me->buttons() & Qt::RightButton ) );
        if( !eatMouseEvents_ ) QGraphicsView::mouseDoubleClickEvent( me );  
    }      

signals:
	void keyPress( int, int, int );
	void keyRelease( int, int, int );
    void mousePress( int, int, int, int, bool, bool, bool );
    void mouseRelease( int, int, int, int, bool, bool, bool );
    void mouseMove( int, int, int, int, bool, bool, bool );
    void mouseDoubleClick( int, int, int, int, bool, bool, bool );
    void closing();
private:
    QPointer< QGraphicsWebView > gwebView_;
private:
	bool eatContextMenuEvent_;
	bool eatKeyEvents_;
    bool eatMouseEvents_;
};

