#include <QObject>
#include <QtPlugin>
#include <QVariantMap>
#include <QPainter>
#include <QRectF>

#include <iostream>

#include <GL/gl.h>
#include <GL/glu.h>

struct IDummy {};
Q_DECLARE_INTERFACE(IDummy,"dummy")

class GLApp : public QObject, public IDummy {
	Q_OBJECT
	Q_INTERFACES( IDummy )
public slots:
    void init( const QVariantMap& args ) {} 
    void render() {
        std::cout << "*";
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor( 0, 0, 0, 1 );
    }
    void resize( int width, int height ) {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluPerspective(70, width / height, 0.01, 1000);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

    }
};

Q_EXPORT_PLUGIN2( opengl-app, GLApp )
