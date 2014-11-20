#include "window.h"
#include "ui_window.h"
#include "utils.h"

Window::Window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::window)
{
    ui->setupUi(this);
}

Window::~Window()
{
    delete ui;
}


void Window::on_menu_item_load_model_triggered()
{
    POLL_DEBUG(std::cout, "clicked on load model");
}


void Window::on_menu_item_exit_triggered()
{
    exit(-1);
}

void Window::on_menu_item_new_scene_triggered()
{

}
