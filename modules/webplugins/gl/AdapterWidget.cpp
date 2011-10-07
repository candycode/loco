/* OpenSceneGraph example, osganimate.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*  THE SOFTWARE.
*/

#include "AdapterWidget.h"

AdapterWidget::AdapterWidget( QWidget * parent, const char * name, const QGLWidget * shareWidget, WindowFlags f):

    QGLWidget(parent, shareWidget, f)

{
    _gw = new osgViewer::GraphicsWindowEmbedded(0,0,width(),height());
    setFocusPolicy(Qt::ClickFocus);

	ticker = 0;
	pressTimer.setInterval(tickInterval);
	connect(&pressTimer, SIGNAL(timeout()), this, SLOT(tick()));
}

void AdapterWidget::resizeGL( int width, int height )
{
    _gw->getEventQueue()->windowResize(0, 0, width, height );
    _gw->resized(0,0,width,height);
}

void AdapterWidget::keyPressEvent( QKeyEvent* event )
{
    _gw->getEventQueue()->keyPress( (osgGA::GUIEventAdapter::KeySymbol) *(event->text().toAscii().data() ) );

}

void AdapterWidget::keyReleaseEvent( QKeyEvent* event )
{
    _gw->getEventQueue()->keyRelease( (osgGA::GUIEventAdapter::KeySymbol) *(event->text().toAscii().data() ) );
}

void AdapterWidget::mousePressEvent( QMouseEvent* event )
{
    int button = 0;
    switch(event->button())
    {
        case(Qt::LeftButton): button = 1; break;
        case(Qt::MidButton): button = 2; break;
        case(Qt::RightButton): button = 3; break;
        case(Qt::NoButton): button = 0; break;
        default: button = 0; break;
    }
    _gw->getEventQueue()->mouseButtonPress(event->x(), event->y(), button);
	pressTimer.start();
	emit pressed(button);
}

void AdapterWidget::tick()
{
	ticker++;
}

void AdapterWidget::mouseReleaseEvent( QMouseEvent* event )
{
    int button = 0;
    switch(event->button())
    {
        case(Qt::LeftButton): button = 1; break;
        case(Qt::MidButton): button = 2; break;
        case(Qt::RightButton): button = 3; break;
        case(Qt::NoButton): button = 0; break;
        default: button = 0; break;
    }
    _gw->getEventQueue()->mouseButtonRelease(event->x(), event->y(), button);
	emit released(event->x(), event->y(), button, ticker);
	pressTimer.stop();
	ticker = 0;
}

void AdapterWidget::mouseMoveEvent( QMouseEvent* event )
{
	/*osgGA::EventQueue::Events eventQueueEvents;
	_gw->getEventQueue()->takeEvents(eventQueueEvents);*/
    _gw->getEventQueue()->mouseMotion(event->x(), event->y());
	emit mouseWasMoved(event);
}

void ViewerQT::resizeGL( int width, int height )
{
	AdapterWidget::resizeGL(width, height);
	emit windowWasResized(width, height);
	if(!continuousUpdate)
		update();

}
void ViewerQT::keyPressEvent( QKeyEvent* event )
{
	AdapterWidget::keyPressEvent( event );
	if(!continuousUpdate)
		update();
}
void ViewerQT::keyReleaseEvent( QKeyEvent* event )
{
	AdapterWidget::keyReleaseEvent( event );
	if(!continuousUpdate)
		update();
}
void ViewerQT::mousePressEvent( QMouseEvent* event )
{
	AdapterWidget::mousePressEvent(event);
	if(!continuousUpdate)
		update();
}
void ViewerQT::mouseReleaseEvent( QMouseEvent* event )
{
	AdapterWidget::mouseReleaseEvent( event );
	if(!continuousUpdate)
		update();
}
void ViewerQT::mouseMoveEvent( QMouseEvent* event )
{
	AdapterWidget::mouseMoveEvent(event);
	if(!continuousUpdate)
		update();
}

void ViewerQT::setContinuousUpdate(bool continuousUpdate_)
{
	continuousUpdate = continuousUpdate_;
	if(continuousUpdate_)
		_timer.start(timerInterval);
	else
		_timer.stop();
}

void ViewerQT::setTimerInterval(int timerInterval_)
{
	timerInterval = timerInterval_;
	if(continuousUpdate)
		_timer.start(timerInterval);
}

osgViewer::Viewer* ViewerQT::getViewer()
{
	return &viewer;
}
