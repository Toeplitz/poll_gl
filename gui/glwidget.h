#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "poll.h"
#define GLEW_STATIC
#include "glew.h"

#include <QGLWidget>
#include <QBasicTimer>
#include <QTimer>

class GLwidget : public QGLWidget
{
  Q_OBJECT


  private:
    Assets assets;
    Poll poll;
    Scene scene;

    QTimer timer;


    bool check_version(const int &major);

  public:
    explicit GLwidget(QWidget *parent = 0);


    /* QT specifics */
    void initializeGL();
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void paintGL();
    void resizeGL(int w, int h);

};

#endif 
