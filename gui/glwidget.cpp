#include "glwidget.h"
#include "gldefaults.h"
#include "utils.h"

#include <QGLFormat>
#include <QMouseEvent>

#include <iostream>



GLwidget::GLwidget(QWidget *parent) :
  QGLWidget(),
  poll()
{

}


void GLwidget::initializeGL()
{
  poll.init();
  scene.init(poll.plugins);
  assets.init(scene);

  QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(updateGL()));
  timer.start(0);
}


void GLwidget::mousePressEvent(QMouseEvent *e)
{
  POLL_DEBUG(std::cout, "Mouse press GLwidget");
}


void GLwidget::mouseReleaseEvent(QMouseEvent *e)
{
  POLL_DEBUG(std::cout, "Mouse release GLwidget");
}

void GLwidget::paintGL()
{
  double dt = poll.delta_time_get();
  poll.profile_fps(dt);

  GL_ASSERT(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
  GL_ASSERT(glClearColor(1.f, 0.0f, 0.0f, 1.f));

}


void GLwidget::resizeGL(int w, int h)
{
  POLL_DEBUG(std::cout, "resizing gl viewport: " << w << " x " << h);
  GL_ASSERT(glViewport(0, 0, (GLint)w, (GLint)h));

  /* change frustnum, framebuffers etc... */

}

