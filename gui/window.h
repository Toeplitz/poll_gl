#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QHBoxLayout>
#include <QMetaType>
#include <QListWidgetItem>
#include <QPixmap>
#include <QIcon>
#include "assets.h"

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

    std::vector<std::unique_ptr<QPixmap>> thumbnails;


    explicit Window(QWidget *parent = 0);
    ~Window();

    void  asset_list_items_populate(const int selection);
    void  asset_list_items_populate_materials(const Material_List &materials);
    void  asset_list_populate();
    Node *node_active_get() { return node_active; }
    void  node_active_set(Node *node) { node_active = node; }
    void  node_details_show();
    void  node_populate(Node *node);
    void  node_tree_populate();

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

    void slot_asset_tree_item_clicked(QListWidgetItem *item);
    void slot_translate_x_changed(double d);

private:

    Ui::window *ui;
};



#endif // WINDOW_H
