#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QHBoxLayout>
#include <QMetaType>


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

    bool     fullMode;
    QWidget *windowParent;
    int      nodes_total = 0;
    long     vertices_total = 0;

private slots:
    void on_button_node_back_clicked();
    void on_menu_item_load_model_triggered();
    void on_menu_item_exit_triggered();
    void on_menu_item_new_scene_triggered();
    void on_menu_item_fullscreen_triggered();
    void on_menu_item_save_poll_scene_triggered();
    void on_menu_item_load_poll_scene_triggered();
    void on_tree_nodes_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void onCustomContextMenu(const QPoint &);
    void showEvent(QShowEvent *);


private:
    Ui::window *ui;
};



#endif // WINDOW_H
