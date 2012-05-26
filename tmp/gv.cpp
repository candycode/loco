/*
 * =====================================================================================
 *
 *       Filename:  gv.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/19/2012 09:17:50 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLabel>
#include <QPixmap>
#include <QGLWidget>

int main( int argc, char** argv ) {

QApplication a(argc, argv);  
  
QGraphicsScene scene;  
scene.addText("Hello, world!");  
scene.addPixmap(QPixmap("./samplemap.png"));  
  
QGraphicsView view(&scene);  
view.setViewport(new QGLWidget); // en commentant ce fonctionne !!  
//view.setViewport (new QGLWidget(new QGLContext(QGLFormat(QGL::Rgba | QGL::DoubleBuffer | QGL::AlphaChannel | QGL::HasOverlay))));  
  
view.show();  
  
QLabel label(&view);  
label.setPixmap(QPixmap("./windsock.png"));  
label.setStyleSheet("background-color: transparent;");  
label.show();  
      
return a.exec();  


}
