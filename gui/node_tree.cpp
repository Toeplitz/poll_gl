#include "node.h"
#include "node_tree.h"
#include "window.h"
#include "utils.h"
#include <QList>
#include <QTreeWidgetItem>
#include <QApplication>
#include <QVariant>
#include <QString>
#include <QTextEdit>
#include <QLineEdit>
#include <QStackedWidget>




Node_Tree::Node_Tree(QObject *parent)
{
}


void Node_Tree::keyPressEvent(QKeyEvent *event)
{
  if (event->key() == Qt::Key_Return) {
    QList<QTreeWidgetItem *> items = this->selectedItems();
    item_selected(items.back());
    POLL_DEBUG(std::cout, "pressed RETURN key on tree widget");

    return;

  }

  QTreeView::keyPressEvent(event);
}


void Node_Tree::item_selected(QTreeWidgetItem *item)
{
  Window *win = (Window *) qApp->activeWindow();
  QStackedWidget *stackedWidget = win->findChild<QStackedWidget *>("stackedWidget");
  QLineEdit *ledit_node_name = win->findChild<QLineEdit *>("ledit_node_name");
  QTextEdit *tedit_summary = win->findChild<QTextEdit *>("tedit_summary");

  QVariant v = item->data(0, Qt::UserRole);
  Node *node = v.value<Node *>();

  POLL_DEBUG(std::cout, "node clicked: " << node->name_get());
  stackedWidget->setCurrentIndex(1);

  QString str = QString::fromUtf8(node->name_get().c_str());
  ledit_node_name->setText(str);

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
    tedit_summary->setText(qsummary);
  }

}
