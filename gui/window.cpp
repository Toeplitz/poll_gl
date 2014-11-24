#include "window.h"
#include "ui_window.h"
#include "utils.h"




Fullscreen_Dialog::Fullscreen_Dialog(QWidget *parent)
{
      /*
  dlg = new QDialog(this);
  dlg_layout = new QHBoxLayout(dlg);
  dlg_layout->setContentsMargins(0, 0, 0, 0);
  dlg->setLayout(dlg_layout);
  dlg->showFullScreen();
  dlg->hide();

      */
  /*
  bool r = connect(dlg, SIGNAL(accepted()), this, SLOT(gl_show_fullscreen()));
  assert(r);

  r = connect(dlg, SIGNAL(rejected()), this, SLOT(gl_show_normal()));
  assert(r);
  */
}


Fullscreen_Dialog::~Fullscreen_Dialog()
{
  if (dlg_layout) delete dlg_layout;
}


void Fullscreen_Dialog::gl_show_fullscreen()
{
  Window *win = (Window *) qApp->activeWindow();
 // QGLWidget *gl_widget = win->findChild<QGLWidget *>("widget");
  dlg_layout = new QHBoxLayout(this);
  dlg_layout->setContentsMargins(0, 0, 0, 0);
  //dlg_layout->addWidget(gl_widget);
  this->setLayout(dlg_layout);
  this->showFullScreen();
  this->show();
  this->grabKeyboard();
  this->grabMouse();
  this->activateWindow();
  this->raise();
  this->setFocus();

}


void Fullscreen_Dialog::gl_show_normal()
{
  /*
  Window *win = (Window *) qApp->activeWindow();
  QHBoxLayout *gl_box = win->findChild<QHBoxLayout *>("gl_hbox");
  QGLWidget *gl_widget = this->findChild<QGLWidget *>("widget");
  gl_box->addWidget(gl_widget);
  this->hide();
  delete dlg;
  dlg_layout = nullptr;
  */

  this->close();
}


void Fullscreen_Dialog::keyPressEvent(QKeyEvent *event)
{
  POLL_DEBUG(std::cout, "key pressed on fullscreen dialog");
  gl_show_normal();
}


Window::Window(QWidget *parent):
        QMainWindow(parent), ui(new Ui::window), fullscreen_dialog(this)
{
  ui->setupUi(this);
}


Window::~Window()
{
  delete ui;
}


void Window::on_menu_item_load_model_triggered()
{
  /*
  Window *win = (Window *) qApp->activeWindow();
  QHBoxLayout *gl_box = win->findChild<QHBoxLayout *>("gl_hbox");
  QGLWidget *gl_widget = win->findChild<QGLWidget *>("widget");

  QDialog *dlg = new QDialog(this);
  QHBoxLayout *dlg_layout = new QHBoxLayout(dlg);
  dlg_layout->setContentsMargins(0, 0, 0, 0);
  dlg_layout->addWidget(gl_widget);
  dlg->setLayout(dlg_layout);
  dlg->showFullScreen();

  bool r = connect(dlg, SIGNAL(accepted()), this, SLOT(showGlNormal()));
  assert(r);
  */
  //r = connect(dlg, SIGNAL(accepted()), this, SLOT(showGlNormal()));
  //assert(r);
}


void Window::showGlNormal()
{
  /*
  Window *win = (Window *) qApp->activeWindow();
  QHBoxLayout *gl_box = win->findChild<QHBoxLayout *>("gl_hbox");
  QGLWidget *gl_widget = win->findChild<QGLWidget *>("widget");
  gl_box->layout()->addWidget(gl_widget);
  */
}


void Window::on_menu_item_exit_triggered()
{
  exit(-1);
}


void Window::on_menu_item_new_scene_triggered()
{

}


void Window::on_menu_item_fullscreen_triggered()
{
  fullscreen_dialog.gl_show_fullscreen();
}

