#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>

namespace Ui {
class window;
}

class Window : public QMainWindow
{
    Q_OBJECT

public:
    explicit Window(QWidget *parent = 0);
    ~Window();



    int height_get() {
        return 1024;
    }

    int width_get() {
        return 1024;
    }

private slots:
    void on_menu_item_load_model_triggered();
    void on_menu_item_exit_triggered();

    void on_menu_item_new_scene_triggered();

private:
    Ui::window *ui;
};

#endif // WINDOW_H
