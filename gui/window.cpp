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
#include <QListWidgetItem>
#include <QMessageBox>
#include "node.h"
#include "scene.h"
#include "texture.h"
#include <memory>




Window::Window(QWidget *parent):
  QMainWindow(parent), ui(new Ui::window)
{
  ui->setupUi(this);
  fullMode = false;
  windowParent = parent;


  connect(ui->list_assets, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(slot_asset_tree_item_clicked(QListWidgetItem *)));
  connect(ui->spin_translate_x, SIGNAL(valueChanged(double)), this, SLOT(slot_translate_x_changed(double)));
}


Window::~Window()
{
  delete ui;
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Window::asset_list_items_populate_materials(const Material_List &materials)
{

  for (auto &material: materials) {
    QListWidgetItem *item = new QListWidgetItem(ui->list_asset_items);

    QString item_text = QString("Material");
    item->setText(item_text);
  }
}


void Window::asset_list_items_populate(const int selection)
{
  Scene &scene = ui->gl_widget->scene_get();
  Assets &assets = scene.assets_get();
  ui->list_asset_items->clear();

  switch (selection) {
    case 1:
      auto &materials = assets.material_get_all();
      asset_list_items_populate_materials(materials);
      break;
  }

}


void Window::asset_list_populate()
{
  Scene &scene = ui->gl_widget->scene_get();
  Assets &assets = scene.assets_get();

  ui->list_assets->clear();

  auto &armatures = assets.armature_get_all();
  const unsigned int num_armatures = armatures.size();
  if (num_armatures > 0) {
    QListWidgetItem *item = new QListWidgetItem(ui->list_assets);
    QString icon_res = tr(":/icons/icons/Bones-icon.png");
    QString item_text = QString("Armatures (%1)").arg(num_armatures);
    item->setText(item_text);
    item->setIcon(QIcon(icon_res));
    item->setData(Qt::UserRole, QVariant(0));
  }

  auto &materials = assets.material_get_all();
  const unsigned int num_materials = materials.size();
  if (num_materials > 0) {
    QListWidgetItem *item = new QListWidgetItem(ui->list_assets);
    QString icon_res = tr(":/icons/icons/Objects-Material-Normal.png");
    QString item_text = QString("Materials (%1)").arg(num_materials);
    item->setText(item_text);
    item->setIcon(QIcon(icon_res));
    item->setData(Qt::UserRole, QVariant(1));
  }

  auto &meshes = assets.mesh_get_all();
  const unsigned int num_meshes = meshes.size();
  if (num_meshes > 0) {
    QListWidgetItem *item = new QListWidgetItem(ui->list_assets);
    QString icon_res = tr(":/icons/icons/Objects-Model-Normal.png");
    QString item_text = QString("Meshes (%1)").arg(num_meshes);
    item->setText(item_text);
    item->setIcon(QIcon(icon_res));
    item->setData(Qt::UserRole, QVariant(2));
  }
}


void Window::showEvent(QShowEvent *)
{
  QMainWindow::show();
  QApplication::processEvents();

  asset_list_populate();
  node_tree_populate();

}


void Window::node_tree_populate()
{
  Scene &scene = ui->gl_widget->scene_get();

  auto tree = ui->tree_nodes;
  tree->populate(scene);

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
  ui->gl_widget->goFullScreen();
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

   // QString qsummary = QString::fromUtf8(summary.c_str());
   // ui->tedit_summary->setText(qsummary);
  }

}


void Window::slot_asset_tree_item_clicked(QListWidgetItem *item)
{
  int selection = item->data(Qt::UserRole).toInt();
  asset_list_items_populate(selection);
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
  node->translate_identity(ui->gl_widget->scene_get(), glm::vec3(d, v.y, v.z));
}
