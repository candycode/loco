#include <glew.h>
//#define GL_GLEXT_PROTOTYPES 1
#include <QObject>
#include <QtPlugin>
#include <QVariantMap>
#ifdef Q_OS_MAC
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <QString>
#include <vector>

struct IDummy {};
Q_DECLARE_INTERFACE(IDummy,"dummy")

class GLApp : public QObject, public IDummy {
	Q_OBJECT
        Q_PROPERTY( QVariantMap bkcolor READ GetBkColor WRITE SetBkColor )
        Q_PROPERTY( QString name READ GetName )
        Q_PROPERTY( QString description READ GetDescription )
	Q_INTERFACES( IDummy )
public:
    GLApp( QObject* parent = 0 ) : QObject( parent ), vertices_( 9, 0.0f ),
                                   vbo_( 0 ), 
                                   r_( 0 ), g_( 0 ), b_( 0 ) {}
    QString GetName() const { return "OpenGL App"; }
    QString GetDescription() const { 
        return "Sample OpenGL application";
    }  
    void SetBkColor( const QVariantMap& c ) {
        if( c.contains( "r" ) ) r_ = c[ "r" ].toInt() / 255.f;
        if( c.contains( "g" ) ) g_ = c[ "g" ].toInt() / 255.f;
        if( c.contains( "b" ) ) b_ = c[ "b" ].toInt() / 255.f;
    }
    QVariantMap GetBkColor() const {
        QVariantMap c;
        c[ "r" ] = int( r_ * 255 );
        c[ "g" ] = int( g_ * 255 );
        c[ "b" ] = int( b_ * 255 );
        return c;
    }
public slots:
    void initGL() {
        GLenum status = glewInit();
        if( status != GLEW_OK ) {
            emit error( QString( "GLEW initialization failed" ) + QString( (char*) glewGetErrorString( status ) ) );
            return;
        }        
        glGenBuffers( 1, &vbo_ );
	glBindBuffer( GL_ARRAY_BUFFER, vbo_ );
	glBufferData( GL_ARRAY_BUFFER, 9 * sizeof( float ), &vertices_[ 0 ], GL_STATIC_DRAW );
        glBindBuffer( GL_ARRAY_BUFFER, 0 ); // <- NEVER EVER FORGET THIS !!!     
    }
    void init( const QVariantMap& ) {
        vertices_[ 0     ] = -1;
        vertices_[ 0 + 1 ] = -1;
        vertices_[ 3     ] =  1;
        vertices_[ 3 + 1 ] = -1;
        vertices_[ 6 + 1 ] =  1; 
    } 
    void render() {
        glClearColor( r_, g_, b_, 1 );
        glClear( GL_COLOR_BUFFER_BIT );
        glEnableVertexAttribArray( 0 );
        glBindBuffer( GL_ARRAY_BUFFER, vbo_ );
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glDrawArrays( GL_TRIANGLES, 0, 3 );
        glBindBuffer( GL_ARRAY_BUFFER, 0 ); // <- NEVER EVER FORGET THIS !!!
        glDisableVertexAttribArray( 0 );
    }
    void resize( int width, int height ) {
        //this is the place where the frustum and world transform
        //are setup e.g:
        //glMatrixMode(GL_PROJECTION);
        //glPushMatrix();
        //glLoadIdentity();
        //gluPerspective(70, width / height, 0.01, 1000);

        //glMatrixMode(GL_MODELVIEW);
        //glPushMatrix();
        //glLoadIdentity();
    }
signals:
    void update();
    void error( const QString& );
private:
    std::vector< float  > vertices_;
    GLuint vbo_;
    float r_;
    float g_;
    float b_;
};

Q_EXPORT_PLUGIN2( opengl-app, GLApp )
