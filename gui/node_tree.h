#ifndef NODE_TREE_H
#define NODE_TREE_H

#include <QTreeWidget>
#include <QKeyEvent>
#include <QMetaType>
#include "node.h"

Q_DECLARE_METATYPE(Node *)

class Scene;


class Node_Tree: public QTreeWidget
{
  Q_OBJECT

private:
  int      nodes_total = 0;
  long     vertices_total = 0;

  void keyPressEvent(QKeyEvent *event);

public:
  explicit Node_Tree(QObject *parent = 0);

  void  item_selected(QTreeWidgetItem *item);
  void  populate(Scene &scene);
  void  populate_recursive(Node &node, QTreeWidget *tree, QTreeWidgetItem *tree_item_in);
  int   nodes_total_get() { return nodes_total; }
  long  vertices_total_get() { return vertices_total; }

signals:

public slots:
  void on_tree_nodes_itemDoubleClicked(QTreeWidgetItem *item, int column);

private slots:
  void on_custom_context_menu(const QPoint &point);

};

#endif // NODE_TREE_H
