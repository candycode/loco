#include <QtGui/QApplication>
#include <QtGui/QLabel>
#include <QtGui/QDialog>
#include <QtGui/QLayout>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsProxyWidget>
#include "QOSGGraphics.h"
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <QtGui>
#include <QtWebKit/QGraphicsWebView>
#include <QtWebKit/QWebHitTestResult>
#include <QtWebKit/QWebElement>
#include <iostream>

class DefTransparencyChecker {
public:
	DefTransparencyChecker( const QString& prop = "opacity", const QString& value = "0" )
		: prop_( prop ), value_( value ) {}

	const QString& property() const { return prop_; }

	bool operator()( const QString& v ) const { return v == value_; }

private:
	QString prop_;
	QString value_;
};

template < typename TransparencyCheckerT = DefTransparencyChecker >
class QTransparentGraphicsWebView : public QGraphicsWebView {
private:
	TransparencyCheckerT transparent_;
	bool HandleEvent( QGraphicsSceneMouseEvent* ev ) const {
		QWebHitTestResult tr = this->page()->currentFrame()
			->hitTestContent( ev->scenePos().toPoint() );
		/*if( tr.isNull() ) {
			std::clog << "ELEMENT null" << std::endl;
			ev->setAccepted( false );
			return false;
		}*/
		//this->mapFromScene(ev->scenePos()).toPoint();
		const QString s = tr.element().styleProperty( transparent_.property(), QWebElement::ComputedStyle );
		std::cout << "ELEMENT opacity: " << s.toStdString() << std::endl;
		std::cout << tr.element().tagName().toStdString() << ' ' << tr.element().localName().toStdString() << ' ' <<
		std::cout << ' ' << ev->scenePos().toPoint().x() << ' ' <<
			ev->scenePos().toPoint().y() << std::endl;
		
		if( tr.element().tagName() == "HTML" ) {
			
			return false; 
		}
		return true;	
	}
protected:
	void mousePressEvent( QGraphicsSceneMouseEvent * ev ) {
		std::cout << "VIEW: MOUSE DOWN" << std::endl;
		if( HandleEvent( ev ) )	QGraphicsWebView::mousePressEvent( ev );
		else ev->setAccepted( false );
	}
	void mouseReleaseEvent( QGraphicsSceneMouseEvent * ev ) {
		std::cout << "VIEW: MOUSE UP" << std::endl;
		if( HandleEvent( ev ) )	{
			QGraphicsWebView::mouseReleaseEvent( ev );
		}
		else ev->setAccepted( false );
		
	}
	void mouseMoveEvent( QGraphicsSceneMouseEvent * ev ) {
		std::cout << "VIEW: MOUSE MOVE" << std::endl;
		if( HandleEvent( ev ) )	QGraphicsWebView::mouseMoveEvent( ev );
		else ev->setAccepted( false );
	}
	void mouseDoubleClickEvent( QGraphicsSceneMouseEvent * ev ) {
		if( HandleEvent( ev ) )	QGraphicsWebView::mouseDoubleClickEvent( ev );
		else ev->setAccepted( false );
	}

};

#if 1
typedef QTransparentGraphicsWebView<> GraphicsWebView;
#else
typedef QGraphicsWebView GraphicsWebView;
#endif

int main(int argc, char *argv[])
{
	if(argc<2)
    {
        std::cout << argv[0] <<": requires filename argument." << std::endl;
        return 1;
    }

	QApplication a(argc, argv);
	osg::QGLGraphicsView view;
    osg::ArgumentParser arguments(&argc, argv);
	QString defURL = "http://www.amazon.com";
	int urlIdx = arguments.find("--url");
	if( urlIdx > 0 ) {
		if( argc < (urlIdx + 2) ) {
			std::cerr << "Missing URL!" << std::endl;
			return 1;
		} else {
			defURL = argv[ urlIdx + 1 ]; 
			arguments.remove( urlIdx + 1 );
		}
	}
	osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFiles(arguments);
    if (!loadedModel)
    {
        std::cout << arguments[0] <<": No data loaded." << std::endl;
        return 1;
    }

	
	if (arguments.read("--composite"))
	{
		osg::QOSGCompositeScene *pScene = new osg::QOSGCompositeScene;
		view.setScene( pScene );

		unsigned int width = /*2 **/ pScene->width();
        unsigned int height = pScene->height();

		{
            osgViewer::View* view1 = new osgViewer::View;
            view1->getCamera()->setGraphicsContext(pScene->getGraphicsWindow());
            view1->getCamera()->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(width/2)/static_cast<double>(height), 1.0, 1000.0);
            view1->getCamera()->setViewport(new osg::Viewport(0,0,width/2,height));
            view1->setCameraManipulator(new osgGA::TrackballManipulator);
            view1->setSceneData(loadedModel.get());
           
            pScene->addView(view1);
      
            osgViewer::View* view2 = new osgViewer::View;
            view2->getCamera()->setGraphicsContext(pScene->getGraphicsWindow());
			view2->getCamera()->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(width/2)/static_cast<double>(height), 1.0, 1000.0);
            view2->getCamera()->setViewport(new osg::Viewport(width/2,0,width/2,height));
            view2->setCameraManipulator(new osgGA::TrackballManipulator);
            view2->setSceneData(loadedModel.get());
            QGraphicsWebView* wv = new GraphicsWebView;
			//wv->setFlag(QGraphicsItem::ItemIsMovable);
			wv->setCacheMode(QGraphicsItem::DeviceCoordinateCache); //without this transparency doensn't work
			wv->setOpacity(0.5);
			wv->settings()->setAttribute(QWebSettings::AcceleratedCompositingEnabled, true);
			wv->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
			wv->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
			wv->settings()->setAttribute(QWebSettings::SpatialNavigationEnabled, true);

			//wv->setAcceptedMouseButtons(0);
			wv->load(defURL);
            pScene->addView(view2);
			pScene->addItem( wv );
        }
	}
	else
	{
		osg::QOSGScene *pScene = new osg::QOSGScene;

		view.setScene( pScene );
		pScene->setContinuousUpdate( 50 );
		pScene->setCameraManipulator(new osgGA::TrackballManipulator);
        pScene->setSceneData(loadedModel.get());
		QGraphicsWebView* wv = new GraphicsWebView;
		wv->setCacheMode(QGraphicsItem::DeviceCoordinateCache); //without this transparency doensn't work
		wv->setOpacity(0.5);
		//wv->setAcceptedMouseButtons(0);
		wv->load(defURL);
		wv->resize( 1024, 768 );
		wv->settings()->setAttribute( QWebSettings::PluginsEnabled, true );
		wv->settings()->setAttribute(QWebSettings::AcceleratedCompositingEnabled, true);
		wv->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
		wv->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
		wv->settings()->setAttribute(QWebSettings::SpatialNavigationEnabled, true);
		wv->setAcceptHoverEvents(true);
		pScene->addItem( wv );
	}

	//// now add a widget on top off all of this
	//QDialog* pDialog = new QDialog( NULL, Qt::CustomizeWindowHint | Qt::WindowTitleHint );
	//pDialog->setWindowOpacity( 0.0 );
	//pDialog->setWindowTitle( "A 'Hello world' widget" );
	//pDialog->setLayout(new QVBoxLayout);
	//pDialog->layout()->addWidget( new QLabel( "Hello OSG users !!!" ) );
	//pDialog->layout()->addWidget( new QPushButton( "Hello OSG users !!!" ) );
	//pDialog->resize(700,500);

	//
	//QGraphicsItem *item = view.scene()->addWidget( pDialog );
	//item->setFlag( QGraphicsItem::ItemIsMovable );	// allow user to move it.
	//item->setCacheMode( QGraphicsItem::DeviceCoordinateCache );	// cached in QPixmap (mapped to OpenGL textures, making the widgets very cheap to redraw).
	//item->setPos( 10, 50 );

	view.show();
	view.resize( 1024, 768 );

	return a.exec();
}

/*
Hi all, 
Since Qt 4.4, we are able to mix OpenGL and Qt in a way which provides 
more possibility than the osgViewerQT do by using QGraphics framework, 
especially for HUD display. 
I joint the example I write to do this. 
The main function loads a model a display into my QGraphicsScene 
inheriting class. 
On top of it is drawn a simple semi-transparent QWidget but we can draw 
whatever we want as menu... 
I extended the example code from the Qt Quaterly to use the osg::Viewer. 
But I think it can interest other Qt/OSG users. 
I still have 2 problems I don't have the time to look forward: 
- The widget transparency only run with the composite viewer (run my 
example with the --composite argument) 
- The window resize produces a deformation of widgets inside but I think 
it more a Qt problem ;) I say this just for information. 
I hope this could be usefull for someone. */

/* JAVASCRIPT TOOLKITS
[V] jQuery UI
[V] DHTMLX
[V] jsTree
[X] extjs - 
[V] syntax highlighter: http://alexgorbatchev.com/SyntaxHighlighter/
[V] http://www.howtocreate.co.uk/jslibs/
[V] qForms http://pengoworks.com/index.cfm?action=get:qforms
[V] qooxdoo
[V] scriptaculous
[V] ALL javascript based 2D viz libraries
*/

/*
//document.elementFromPoint(2, 2);

bool HandleEvent() const {
	QWebHitTestResult tr = this->page()->currentFrame()
							->hitTestContent( ev->buttonDownPos().toPoint() );
	if( tr.isNull() ) return;
	const QString s = tr.element().styleProperty( "opacity", QWebElement::ComputedStyle );
	if( s != "0" ) return true;
	//OR
	//const float OPACITY_THREASHOLD = 0.1;
	//if( s.toFloat() <  OPACITY_THRESHOLD ) QGraphicsWebView::mousePressEvent( ev );
	ev->setAccepted( false );
	return false;	
}

void QTransparentGraphicsWebView::mousePressEvent( QGraphicsSceneMouseEvent * ev ) {
	if( HandleEvent( ev ) )	QGraphicsWebView::mousePressEvent( ev );
	
}
*/


