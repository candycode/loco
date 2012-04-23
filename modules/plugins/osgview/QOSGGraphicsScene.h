#pragma once
//#SRCHEADER

#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgDB/ReadFile>

#include <osgGA/StateSetManipulator>

#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtGui/QKeyEvent>
#include <QtOpenGL/QGLWidget>
#include <QtGui/QGraphicsView>
#include <QtGui/QGraphicsScene>
#include <QTimer>

//using Qt::WindowFlags;

namespace osg
{
//==============================================================================
    /** Adapte Qt events to OSG. */
    class QAdapterScene : public QGraphicsScene
    {
    public:
        QAdapterScene(QObject* parent = 0);
        QAdapterScene( const QRectF & sceneRect, QObject * parent = 0 );
        QAdapterScene( qreal x, qreal y, qreal width, qreal height, QObject * parent = 0 );

        osgViewer::GraphicsWindow* getGraphicsWindow() { return _gw.get(); }
        const osgViewer::GraphicsWindow* getGraphicsWindow() const { return _gw.get(); }
        virtual void resizeEvent( QResizeEvent* event );

        static osgGA::GUIEventAdapter::KeySymbol convertSymbol(int);

    protected:
        virtual void keyPressEvent( QKeyEvent* event );
        virtual void keyReleaseEvent( QKeyEvent* event );
        virtual void mousePressEvent( QGraphicsSceneMouseEvent* event );
        virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent* event );
        virtual void mouseMoveEvent( QGraphicsSceneMouseEvent* event );

    protected:
        osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> _gw;
    };
//==============================================================================
    /** OSG and OpenGL staff.
     *  Draw OpenGL as background, all widgets or else are drawn on top (see widget opacity). */
    class QOSGScene : public QAdapterScene, public osgViewer::Viewer
    {
    public:
        QOSGScene(QObject* parent = 0);
        QOSGScene( const QRectF & sceneRect, QObject * parent = 0 );
        QOSGScene( qreal x, qreal y, qreal width, qreal height, QObject * parent = 0 );
        virtual void drawBackground ( QPainter * painter, const QRectF & rect );
        void setContinuousUpdate( int ms ) {
            if( ms <= 0 ) {
                _timer.stop();
                QObject::disconnect( &_timer, SIGNAL(timeout()), this, SLOT(update()) );
            } else {
                QObject::connect( &_timer, SIGNAL(timeout()), this, SLOT(update()) );
                _timer.start( ms );
            }
        }
        bool LoadScene( const QString& path ) {
            osg::Node* scene = osgDB::readNodeFile( path.toStdString() );
            if( !scene ) return false;
            setSceneData( scene );
            return true;
        }
        void Resize( int w, int h );
        void Wireframe() { ssetManip_->setPolygonMode( osg::PolygonMode::LINE ); }
        void Point() { ssetManip_->setPolygonMode( osg::PolygonMode::POINT ); } 
        void Solid() { ssetManip_->setPolygonMode( osg::PolygonMode::FILL ); }
    protected:
        virtual void init(qreal x, qreal y, qreal width, qreal height);
        osg::ref_ptr< osg::StateSet > m_lastStateSet;
        bool firstFrame;
        QTimer _timer;
    private:
        osg::ref_ptr< osgGA::StateSetManipulator > ssetManip_;
    };
//==============================================================================
    /** OSG and OpenGL stuff.
     *  Draw OpenGL as background, all widgets or else are drawn on top (see widget opacity). */
    class QOSGCompositeScene : public QAdapterScene, public osgViewer::CompositeViewer
    {
    public:
        QOSGCompositeScene(QObject* parent = 0);
        QOSGCompositeScene( const QRectF & sceneRect, QObject * parent = 0 );
        QOSGCompositeScene( qreal x, qreal y, qreal width, qreal height, QObject * parent = 0 );
        virtual void drawBackground ( QPainter * painter, const QRectF & rect );
        void addView(osgViewer::View* view) { 
            if( !view ) return;
            osgViewer::CompositeViewer::addView( view );
            unsigned idx = 0;
            for( idx; idx != getNumViews() && getView(idx) != view; ++idx );
            if( idx >= getNumViews() ) return; //assert!
            //m_lastStateSet.resize( m_lastStateSet.size() + 1 );
            osg::ref_ptr< osg::StateSet > set = new osg::StateSet;
            getView(idx)->getCamera()->getGraphicsContext()->getState()->captureCurrentState( *set );
            m_lastStateSet.insert( m_lastStateSet.begin() + idx, set ); 

        }
        void removeView(osgViewer::View* view) {
            if( !view ) return;
            osgViewer::CompositeViewer::removeView( view );
            unsigned idx = 0;
            for( idx; idx != getNumViews() && getView(idx) != view; ++idx );
            m_lastStateSet.erase( m_lastStateSet.begin() + idx );
        }
        /// @todo
        ///void setContinuousUpdate( int msec ); //if msec <=0 disable continuous update
    protected:
        typedef std::vector< osg::ref_ptr< osg::StateSet > > STATE_ARRAY;
        virtual void init();
        STATE_ARRAY m_lastStateSet;
        size_t frameCount;
        QTimer _timer;
    };

};
