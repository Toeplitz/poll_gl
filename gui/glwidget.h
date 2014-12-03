#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "poll.h"
#include "scene.h"
#define GLEW_STATIC
#include "glew.h"

#include <QGLWidget>
#include <QBasicTimer>
#include <QTimer>

#include <memory>


class GLwidget : public QGLWidget
{
  Q_OBJECT


  private:
    Poll poll;
    Scene scene;

    QTimer timer;

    std::unique_ptr<Poll_Plugin> plugin_firstperson_camera;


  public:
    explicit GLwidget(QWidget *parent = 0);


    /* QT specifics */
    void initializeGL();
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void paintGL();
    void resizeGL(int w, int h);

public:
    Scene& getScene(){return scene;}

    void goFullScreen();
    void backFromFullScreen();

};

#endif 
