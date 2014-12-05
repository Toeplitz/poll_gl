#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QHBoxLayout>


namespace Ui {
class window;
}


class Node;
class QTreeWidget;
class QTreeWidgetItem;
class QLayoutItem;

class Window : public QMainWindow
{
    Q_OBJECT

public:
    explicit Window(QWidget *parent = 0);
    ~Window();

    void scene_tree_fill();
    void node_recursive_load_tree(Node &, QTreeWidget * , QTreeWidgetItem *tree_item_in);

    bool fullMode;
    QWidget *windowParent;

private slots:
    void on_menu_item_load_model_triggered();
    void on_menu_item_exit_triggered();
    void on_menu_item_new_scene_triggered();
    void on_menu_item_fullscreen_triggered();
    void on_menu_item_save_poll_scene_triggered();
    void on_menu_item_load_poll_scene_triggered();
    void on_tree_nodes_doubleClicked(const QModelIndex &index);
    void on_tree_nodes_activated(const QModelIndex &index);

    void onCustomContextMenu(const QPoint &);
    void showEvent(QShowEvent *);

private:
    Ui::window *ui;
};

#endif // WINDOW_H
