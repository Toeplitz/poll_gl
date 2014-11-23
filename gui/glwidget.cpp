#include "glwidget.h"
#include "gldefaults.h"
#include "poll.h"
#include "utils.h"

#include <QGLFormat>
#include <QMouseEvent>

#include <iostream>


QGLFormat fmt;

QGLFormat *foo()
{
  fmt.setVersion(3, 3);
  fmt.setProfile(QGLFormat::CoreProfile);
  fmt.setDoubleBuffer(true);
  fmt.setDepthBufferSize(24);
  fmt.setStencilBufferSize(8);
  return &fmt;

}


GLwidget::GLwidget(QWidget *parent) :
        QGLWidget(*foo()),
        poll(),
        scene()
{

}


void GLwidget::initializeGL()
{
  std::cout << "Widget OpenGl: " << format().majorVersion() << "." << format().minorVersion() << std::endl;
  std::cout << "Context valid: " << context()->isValid() << std::endl;

  poll.init();
  scene.init(poll);

  const float scene_scalar = 0.7;

  Node &root = scene.node_root_get();
  root.scale(scene, glm::vec3(scene_scalar, scene_scalar, scene_scalar));


  {
    Node &floor = scene.load("data/demo_room/floor/", "floor.obj", MODEL_IMPORT_OPTIMIZED );
    Material *material = floor.material_get();
    vec3 v(0.5, 0.5, 0.5);
    material->color_set(v, v, v, 1);
  }

  {
    Node &backdrop= scene.load("data/demo_room/backdrop/", "backdrop.dae", MODEL_IMPORT_OPTIMIZED);
    backdrop.rotate(scene, M_PI, vec3(0, 1, 0));
    backdrop.translate(scene, vec3(-25.f * scene_scalar, 0, 0));

    Node &zombie = scene.load("data/zombie/", "new_thin_zombie.dae", MODEL_IMPORT_OPTIMIZED);
    zombie.rotate(scene, (float) M_PI / 2.f, vec3(0, 0, 1));
    zombie.translate(scene, vec3(-18 * scene_scalar, -12.5 * scene_scalar, 9 * scene_scalar));
  }

  {
    Node &backdrop= scene.load("data/demo_room/backdrop/", "backdrop.dae", MODEL_IMPORT_OPTIMIZED);
    backdrop.rotate(scene, M_PI, vec3(0, 1, 0));
    backdrop.translate(scene, vec3(-10.f * scene_scalar, 0, 0));

    Node &teapot= scene.load("data/demo_room/teapot/", "teapot.obj", MODEL_IMPORT_DEFAULT);
    teapot.translate(scene, vec3(-8 * scene_scalar, 2.5 * scene_scalar, 0));
    teapot.scale(scene, vec3(0.02, 0.02, 0.02));

    Node &teapot_no_shadow = scene.load("data/demo_room/teapot/", "teapot.obj", MODEL_IMPORT_DEFAULT);
    teapot_no_shadow.translate(scene, vec3(-12 * scene_scalar, 2.5 * scene_scalar, 0));
    teapot_no_shadow.scale(scene, vec3(0.02, 0.02, 0.02));
    for (auto &child: teapot_no_shadow.children_get()) {
      child->shadow_cast_set(scene, true);
    }
  }

   {
    Node *node = scene.node_create("Light_Directionl_Global");
    Light *light = node->light_create(scene, Light::DIRECTIONAL, Light::GLOBAL);
    node->translate(scene, glm::vec3(0, 40, 0));
    light->properties_direction_set(glm::vec3(0, -1, -1));
    light->properties_color_set(glm::vec3(0.5, 0.5, 0.5));
  }

  {
    Node &mitsuba = scene.load("data/demo_room/mitsuba/", "mitsuba.obj", MODEL_IMPORT_DEFAULT);
    mitsuba.translate(scene, vec3(5 * scene_scalar, 0, 0));
  }

  {
    Node &mitsuba = scene.load("data/demo_room/mitsuba/", "mitsuba.obj", MODEL_IMPORT_DEFAULT);
    mitsuba.translate(scene, vec3(20.f * scene_scalar, 0, 0));
  }

  scene.scene_graph_print(true);

  QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(updateGL()));
  timer.start(0);

  this->setFocusPolicy(Qt::StrongFocus);
  this->setFocus();
}


void GLwidget::keyPressEvent(QKeyEvent *event)
{
  POLL_DEBUG(std::cout, "Key press: " << event->key());
}


void GLwidget::mousePressEvent(QMouseEvent *e)
{
  POLL_DEBUG(std::cout, "Mouse press GLwidget");
}


void GLwidget::mouseMoveEvent(QMouseEvent *e)
{
  POLL_DEBUG(std::cout, "Mouse move: " << e->globalX() << ", " << e->globalY());
}


void GLwidget::mouseReleaseEvent(QMouseEvent *e)
{
  POLL_DEBUG(std::cout, "Mouse release GLwidget");
}


void GLwidget::paintGL()
{
 poll.step(scene);
}


void GLwidget::resizeGL(int w, int h)
{
  GLcontext &glcontext = poll.glcontext_get();

  if (w <= 0 || h <= 0) return;

  POLL_DEBUG(std::cout, "resizing gl viewport: " << w << " x " << h);

  scene.resize(nullptr, w, h);
  glcontext.resize(w, h);

  /* change frustnum, framebuffers etc... */
}

