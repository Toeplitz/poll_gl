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
    bool     fullMode;
    QWidget *windowParent;
    Node    *node_active = nullptr;


    explicit Window(QWidget *parent = 0);
    ~Window();

    void  tree_populate();
    Node *node_active_get() { return node_active; }
    void  node_active_set(Node *node) { node_active = node; }
    void  node_details_show();
    void  node_populate(Node *node);

private slots:
    void on_button_node_back_clicked();
    void on_menu_item_load_model_triggered();
    void on_menu_item_exit_triggered();
    void on_menu_item_new_scene_triggered();
    void on_menu_item_fullscreen_triggered();
    void on_menu_item_save_poll_scene_triggered();
    void on_menu_item_load_poll_scene_triggered();

    void showEvent(QShowEvent *);

    void on_button_select_parent_node_clicked();

    void slot_translate_x_changed(double d);

private:

    Ui::window *ui;
};



#endif // WINDOW_H
