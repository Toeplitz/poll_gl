#ifndef NODE_TREE_H
#define NODE_TREE_H

#include <QTreeWidget>
#include <QKeyEvent>
#include <QMetaType>
#include "node.h"

Q_DECLARE_METATYPE(Node *)

class Node_Tree: public QTreeWidget
{
  Q_OBJECT

public:
  explicit Node_Tree(QObject *parent = 0);

  void item_selected(QTreeWidgetItem *item);

private:
  void keyPressEvent(QKeyEvent *event);

signals:

public slots:
  void on_tree_nodes_itemDoubleClicked(QTreeWidgetItem *item, int column);

};

#endif // NODE_TREE_H
