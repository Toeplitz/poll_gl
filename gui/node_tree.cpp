#include "node.h"
#include "node_tree.h"
#include "glwidget.h"
#include "window.h"
#include "scene.h"
#include "utils.h"
#include <QList>
#include <QTreeWidgetItem>
#include <QApplication>
#include <QVariant>
#include <QString>
#include <QTextEdit>
#include <QLineEdit>
#include <QStackedWidget>
#include <QFile>
#include <QLabel>
#include <QMenu>
#include <QDebug>


Node_Tree::Node_Tree(QObject *parent)
{
  connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
          this, SLOT(on_tree_nodes_itemDoubleClicked(QTreeWidgetItem *, int)));
}


void Node_Tree::keyPressEvent(QKeyEvent *event)
{
  if (event->key() == Qt::Key_Return) {
    QList<QTreeWidgetItem *> items = this->selectedItems();
    emit itemDoubleClicked(items.back(), 0);
    return;
  }

  QTreeView::keyPressEvent(event);
}


void Node_Tree::item_selected(QTreeWidgetItem *item)
{
  Window *win = (Window *) qApp->activeWindow();

  QVariant v = item->data(0, Qt::UserRole);
  Node *node = v.value<Node *>();

  win->node_populate(node);
  win->node_details_show();

}


void Node_Tree::on_custom_context_menu(const QPoint &point)
{
  QTreeWidgetItem *item = this->itemAt(point);
  if(item) {
    QMenu contextMenu;
    contextMenu.addAction(item->text(0).toStdString().c_str());
    contextMenu.exec(this->mapToGlobal(point));
  }
}


void Node_Tree::populate(Scene &scene)
{
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
          this, SLOT(on_custom_context_menu(const QPoint &)));
  this->clear();

  Node &root = scene.node_root_get();
  vertices_total = 0;
  nodes_total = 0;

  for (auto &child : root.children_get()) {
    populate_recursive(*child, this, NULL);
  }

}


void Node_Tree::populate_recursive(Node &node, QTreeWidget *tree, QTreeWidgetItem *tree_item_in)
{
  QTreeWidgetItem *tree_item = new QTreeWidgetItem();
  nodes_total++;

  Camera *camera = node.camera_get();
  Light *light = node.light_get();
  Mesh *mesh = node.mesh_get();

  QString icon_string = tr(":/icons/icons/node.png");

  if (camera) {
    icon_string = tr(":/icons/icons/camera-lens.png");
  } else if (light) {
    icon_string = tr(":/icons/icons/light-bulb.png");
  } else if (mesh) {
    icon_string = tr(":/icons/icons/Objects-Model-Normal.png");
  }


  tree_item->setText(0, node.name_get().c_str());

  /*
  if (mesh) {
    tree_item->setText(1, tr(std::to_string(mesh->num_vertices_get()).c_str()));
    vertices_total += mesh->num_vertices_get();
  } else {
    tree_item->setText(1, "0");
  }
  */


  QVariant v;
  v.setValue(&node);
  tree_item->setData(0, Qt::UserRole, v);


  if (icon_string.length() > 0) {
    if (!QFile(icon_string).exists()) {
      qDebug() << "wrong file name : " << icon_string;
    } else {
      tree_item->setIcon(0, QIcon(icon_string));
    }
  } else {
    tree_item->setIcon(0, QIcon());
  }

  //material->diffuse->filename
  //tree_item->setIcon(2, QIcon(tr(material->diffuse->filename.c_str())));

  if(tree_item_in)
    tree_item_in->addChild(tree_item);
  else
    tree->addTopLevelItem(tree_item);
  for (auto &child : node.children_get()) {
    populate_recursive(*child, tree, tree_item);
  }
}



void Node_Tree::on_tree_nodes_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
  item_selected(item);
}

