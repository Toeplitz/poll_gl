#include "glwidget.h"
#include "gldefaults.h"
#include "utils.h"


#include <iostream>

GLwidget::GLwidget(QWidget *parent) :
  QGLWidget(parent),
  poll()
{

}


void GLwidget::initializeGL()
{
  poll.init();
}


void GLwidget::paintGL()
{
  POLL_DEBUG(std::cout, "QT GL repaint");
  GL_ASSERT(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
  GL_ASSERT(glClearColor(1.f, 0.0f, 0.0f, 1.f));

}


void GLwidget::resizeGL(int w, int h)
{

}


