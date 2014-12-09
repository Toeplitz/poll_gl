#include "window.h"
#include "ui_window.h"
#include "utils.h"

#include <QTreeWidgetItem>
#include <QFileDialog>
#include <QMenu>
#include <QIcon>
#include <QPixmap>
#include <QDebug>
#include <QVariant>
#include <QMessageBox>
#include "node.h"
#include "scene.h"
#include "texture.h"




Window::Window(QWidget *parent):
  QMainWindow(parent), ui(new Ui::window)
{
  ui->setupUi(this);
  fullMode = false;
  windowParent = parent;


  connect(ui->spin_translate_x, SIGNAL(valueChanged(double)), this, SLOT(slot_translate_x_changed(double)));
}


Window::~Window()
{
  delete ui;
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Window::showEvent(QShowEvent *)
{
  QMainWindow::show();
  QApplication::processEvents();

  tree_populate();
}


void Window::tree_populate()
{
  Scene &scene = ui->widget->scene_get();

  auto tree = ui->tree_nodes;
  tree->populate(scene);

  ui->label_total_vertices->setText(tr(std::to_string(tree->vertices_total_get()).c_str()));
  ui->label_num_nodes->setText(tr(std::to_string(tree->nodes_total_get()).c_str()));
}


/**************************************************/
/***************** PRIVATE METHODS ****************/
/**************************************************/


void Window::on_button_node_back_clicked()
{
  node_active_set(nullptr);
  ui->stackedWidget->setCurrentIndex(0);
}


void Window::on_menu_item_load_model_triggered()
{
  QString fileFormats = tr("Model Files (*.obj)");
  QString openStartPoint = "/home/mike/Documents/";

  QString fileName = QFileDialog::getOpenFileName(NULL,
                                                  tr("Select Model file..."), openStartPoint, fileFormats);

  POLL_DEBUG(std::cout, "selected model file:");
  POLL_DEBUG(std::cout, fileName.toStdString());
}


void Window::on_menu_item_exit_triggered()
{
  QApplication::quit();
}


void Window::on_menu_item_new_scene_triggered()
{

}


void Window::on_menu_item_fullscreen_triggered()
{
  GLwidget *gl_widget = findChild<GLwidget *>("widget");
  if(gl_widget)
    gl_widget->goFullScreen();
}


void Window::on_menu_item_save_poll_scene_triggered()
{
  QString fileFormats = tr("Scene Files (*.obj)");
  QString openStartPoint = "/home/mike/Documents/";

  QString fileName = QFileDialog::getOpenFileName(NULL,
                                                  tr("Save as Scene file..."), openStartPoint, fileFormats);

  POLL_DEBUG(std::cout, "save to scene file:");
  POLL_DEBUG(std::cout, fileName.toStdString());

}


void Window::on_menu_item_load_poll_scene_triggered()
{
  // adapted from http://qt-project.org/doc/qt-5/qfiledialog.html
  QString fileFormats = tr("Scene Files (*.obj)");
  QString openStartPoint = "/home/mike/Documents/";

  QString fileName = QFileDialog::getOpenFileName(NULL,
                                                  tr("Select Scene file..."), openStartPoint, fileFormats);

  POLL_DEBUG(std::cout, "selected scene file:");
  POLL_DEBUG(std::cout, fileName.toStdString());
}


void Window::on_button_select_parent_node_clicked()
{
  Node *node = node_active_get();
  if (!node) return;

  Node *parent = node->parent_get();
  if (!parent) return;

  node_populate(parent);
}


void Window::node_details_show()
{
  ui->stackedWidget->setCurrentIndex(1);
}


void Window::node_populate(Node *node)
{

  if (node) {
    node_active_set(node);
  } else {
    node_active_set(nullptr);
    POLL_DEBUG(std::cerr, "no node!");
    return;
  }

  /* Fill main text edit */
  {
    QString str = QString::fromUtf8(node->name_get().c_str());
    ui->ledit_node_name->setText(str);
  }

  {
    glm::vec3 v = node->position_get();
    ui->spin_translate_x->setValue(v.x);
    ui->spin_translate_y->setValue(v.y);
    ui->spin_translate_z->setValue(v.z);


  }

  /* Fill text area */
  {
    std::string summary;

    if (node->material_get()) {
      summary += "Material\n";
    }
    if (node->light_get()) {
      summary += "Light\n";
    }


    Mesh *mesh = node->mesh_get();
    if (mesh) {
      summary += "Vertices: " + std::to_string(mesh->num_vertices_get()) + "\n";
      summary += "Indices: " + std::to_string(mesh->num_indices_get()) + "\n";
      summary += "Texture coordinates: " + std::to_string(mesh->num_texture_st_get()) + "\n";
    }

    QString qsummary = QString::fromUtf8(summary.c_str());
    ui->tedit_summary->setText(qsummary);
  }

}


void Window::slot_translate_x_changed(double d)
{
  Node *node = node_active_get();

  if (!node) {
    QMessageBox msgBox;
    msgBox.setText("No active node!");
    msgBox.exec();
    return;
  }

  glm::vec3 v = node->position_get();
  node->translate_identity(ui->widget->scene_get(), glm::vec3(d, v.y, v.z));
}
