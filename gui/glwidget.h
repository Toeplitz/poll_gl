#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "poll.h"
#define GLEW_STATIC
#include "glew.h"

#include <QGLWidget>

class GLwidget : public QGLWidget
{
    Q_OBJECT


private:
    Poll poll;


    bool check_version(const int &major);

public:
    explicit GLwidget(QWidget *parent = 0);
    void check_error();
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

};

#endif // GLWIDGET_H
