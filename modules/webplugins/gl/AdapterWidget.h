#pragma once
//#SRCHEADER

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>

#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtGui/QKeyEvent>
#include <QtOpenGL/QGLWidget>

/// Widget which dispatches events to osg viewer.
class AdapterWidget : public QGLWidget
{
    Q_OBJECT
    public:
		/// Constructor

		/// \param parent parent widget.
		/// \param name widget name.
		/// \param shareWidget share widget, see Qt documentation.
		/// \param f window flags, see Qt documentation.
        AdapterWidget( QWidget * parent = 0, const char * name = 0, const QGLWidget * shareWidget = 0, WindowFlags f = 0);

		/// Destructor
        virtual ~AdapterWidget() {}

		/// Returns the graphics window.

        osgViewer::GraphicsWindow* getGraphicsWindow() { return _gw.get(); }
		/// Returns the graphics window.
        const osgViewer::GraphicsWindow* getGraphicsWindow() const { return _gw.get(); }

	signals:
		/// Emitted whenever a mouse button is pressed.

		/// \param button Code for pressed mouse button (1: left, 2:middle, 3:right).
		void pressed(int button);
		/// Emitted whenever a mouse button which was pressed is released.

		/// \param x X coordinate of mouse click.
		/// \param y Y coordinate of mouse click.
		/// \param button Code for pressed mouse button (1: left, 2:middle, 3:right).
		/// \param pressedTicks the number of ticks passed during the last mouse pression. Each tick has a duration defined in msecs in AdapterWidget::tickInterval.
		void released(int x, int y, int button, int pressedTicks);

		/// Emitted whenever the mouse is moved.
		void mouseWasMoved( QMouseEvent* event );

	protected:
		/// Initialize.
        void init();
		/// Resize GL window.
        virtual void resizeGL( int width, int height );
		/// Key press event.

		/// \param event pressed key event.
        virtual void keyPressEvent( QKeyEvent* event );
		/// Key release event.

		/// \param event released key event.
        virtual void keyReleaseEvent( QKeyEvent* event );
		/// Mouse press event.

		/// \param event pressed mouse event. Emit AdapterWidget::pressed() signal.
        virtual void mousePressEvent( QMouseEvent* event );
		/// Mouse release event.

		/// \param event released mouse event. Emit AdapterWidget::released() signal.
        virtual void mouseReleaseEvent( QMouseEvent* event );
		/// Mouse move event.

		/// \param event mouse move event.
        virtual void mouseMoveEvent( QMouseEvent* event );
		/// Pointer to embedded osg graphics window.
        osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> _gw;
		/// Timer to update ticker every time a AdapterWidget::tickInterval passes.
		QTimer pressTimer;
		/// Simple counter of pressed time interval.
		int ticker;
		/// Amount of time (in msecs) which passes between two ticks.
		static const int tickInterval = 25;

	protected slots:
		/// Increment ticker.
		void tick();
};

/// Widget containing a viewer.
class ViewerQT : public AdapterWidget
{
	Q_OBJECT
    public:
		/// Constructor

		/// \param parent parent widget.
		/// \param name widget name.
		/// \param shareWidget share widget, see Qt documentation.
		/// \param f window flags, see Qt documentation.
		/// \param continuousUpdate_ if set to true, the window is continuously updated (animated).
		/// \param timerInterval_ amount of time (in msecs) between to subsequent scree updates in continuous update mode.
        ViewerQT(QWidget * parent = 0, const char * name = 0, const QGLWidget * shareWidget = 0, WindowFlags f = 0, bool continuousUpdate_ = false, int timerInterval_ = 10):
			AdapterWidget( parent, name, shareWidget, f), timerInterval(timerInterval_), continuousUpdate(continuousUpdate_)
        {
			viewer.getCamera()->setViewport(new osg::Viewport(0,0,width(),height()));
            viewer.getCamera()->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(width())/static_cast<double>(height()), 1.0f, 10000.0f);
            viewer.getCamera()->setGraphicsContext(getGraphicsWindow());

            viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);

            connect(&_timer, SIGNAL(timeout()), this, SLOT(update()));
			if (continuousUpdate_)
				_timer.start(timerInterval);
        }

		/// Refresh viewer window.
        virtual void paintGL()
        {
            viewer.frame();
        }

		/// Returns pointer to viewer;
		osgViewer::Viewer* getViewer();

		/// Timer for refreshing.
        QTimer _timer;
		/// Amount of time between two subsequent screen updates.
		int timerInterval;
		/// If set to true, the screen is continuously updated (animated).
		bool continuousUpdate;

	signals:
		void windowWasResized(int width, int height);

	private:
		/// Scene viewer.
		osgViewer::Viewer viewer;

	protected slots:
		/// Sets ViewerQT::continuousUpdate.
		void setContinuousUpdate(bool continuousUpdate_);
		/// Sets ViewerQT::timerInterval.
		void setTimerInterval(int timerInterval_);

	protected:
		/// Resize GL window.

		/// \arg width window width.
		/// \arg height window height.
        void resizeGL( int width, int height );
		/// Key press event.

		/// \param event pressed key event.
        virtual void keyPressEvent( QKeyEvent* event );
		/// Key release event.

		/// \param event released key event.
        virtual void keyReleaseEvent( QKeyEvent* event );
		/// Mouse press event.

		/// \param event pressed mouse event. Emit AdapterWidget::pressed() signal.
        virtual void mousePressEvent( QMouseEvent* event );
		/// Mouse release event.

		/// \param event released mouse event. Emit AdapterWidget::released() signal.
        virtual void mouseReleaseEvent( QMouseEvent* event );
		/// Mouse move event.

		/// \param event mouse move event.
        virtual void mouseMoveEvent( QMouseEvent* event );

};
