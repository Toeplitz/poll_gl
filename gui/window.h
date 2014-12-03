#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QHBoxLayout>


namespace Ui {
class window;
}


//class Fullscreen_Dialog: public QDialog
//{
//  Q_OBJECT

//  public:
//    Fullscreen_Dialog(QWidget *parent = 0);
//    ~Fullscreen_Dialog();

//    QDialog *dlg;
//    QHBoxLayout *dlg_layout;

//  public:
//    void gl_show_fullscreen();
//    void gl_show_normal();

//  private:
//    void keyPressEvent(QKeyEvent *event);
//};

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

    void dataExchange();
    void node_recursive_load_tree(Node &, QTreeWidget* , QTreeWidgetItem* );


    int height_get() {
        return 1024;
    }

    int width_get() {
        return 1024;
    }

    bool fullMode;
    QWidget *windowParent;

private slots:
    void on_menu_item_load_model_triggered();
    void on_menu_item_exit_triggered();
    void showGlNormal();

    void on_menu_item_new_scene_triggered();

    void on_menu_item_fullscreen_triggered();

    void on_actionLoad_poll_scene_triggered();

    void on_actionSave_poll_scene_triggered();

    void on_updateTree_clicked();

    void onCustomContextMenu(const QPoint &);

private:
//    Fullscreen_Dialog fullscreen_dialog;
    Ui::window *ui;
};

#endif // WINDOW_H
