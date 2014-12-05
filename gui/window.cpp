#include "window.h"
#include "ui_window.h"
#include "utils.h"

#include <QTreeWidgetItem>
#include <QFileDialog>
#include <QMenu>
#include <QIcon>
#include <QPixmap>
#include <QDebug>



Window::Window(QWidget *parent):
  QMainWindow(parent), ui(new Ui::window)
{
    ui->setupUi(this);
    fullMode = false;
    windowParent = parent;
}


Window::~Window()
{
    delete ui;
}


void Window::scene_tree_fill()
{
    GLwidget *gl_widget = findChild<GLwidget *>("widget");
    QTreeWidget *treeWidget = findChild<QTreeWidget *>();

    treeWidget ->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(treeWidget, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(onCustomContextMenu(const QPoint &)));
    treeWidget->clear();

    Scene &widgetScene = gl_widget->getScene();
    Node &root = widgetScene.node_root_get();

    for (auto &child : root.children_get()) {
        node_recursive_load_tree(*child, treeWidget, NULL);
    }
}


void Window::node_recursive_load_tree(Node &node, QTreeWidget *tree,
                                      QTreeWidgetItem *tree_item_in)
{
    QTreeWidgetItem *tree_item = new QTreeWidgetItem();

    Camera *camera = node.camera_get();
    Light *light = node.light_get();

    QString icon_string = tr(":/icons/icons/node.png");

    if (camera) {
        icon_string = tr(":/icons/icons/camera-lens.png");
    } else if (light) {
        icon_string = tr(":/icons/icons/light-bulb.png");
    }

    tree_item->setText(0, node.name_get().c_str());

    if (!QFile(icon_string).exists()) {
      qDebug() << "wrong file name : " << icon_string;
    } else {
      tree_item->setIcon(0, QIcon(icon_string));
      qDebug() << "Setting icon: " << icon_string;
    }


    if(tree_item_in)
        tree_item_in->addChild(tree_item);
    else
        tree->addTopLevelItem(tree_item);
    for (auto &child : node.children_get()) {
        node_recursive_load_tree(*child, tree, tree_item);
    }
}


void Window::on_menu_item_load_model_triggered()
{
    // adapted from http://qt-project.org/doc/qt-5/qfiledialog.html
    QString fileFormats = tr("Model Files (*.obj)");
    QString openStartPoint = "/home/mike/Documents/";

    QString fileName = QFileDialog::getOpenFileName(NULL,
                                                    tr("Select Model file..."), openStartPoint, fileFormats);

    POLL_DEBUG(std::cout, "selected model file:");
    POLL_DEBUG(std::cout, fileName.toStdString());

}


void Window::on_menu_item_exit_triggered()
{
    QApplication::quit();
}


void Window::on_menu_item_new_scene_triggered()
{

}


void Window::on_menu_item_fullscreen_triggered()
{
    GLwidget *gl_widget = findChild<GLwidget *>("widget");
    if(gl_widget)
        gl_widget->goFullScreen();
}


void Window::onCustomContextMenu(const QPoint &point)
{
    QTreeWidget *treeWidget = findChild<QTreeWidget *>();

    if(treeWidget)
    {
        QTreeWidgetItem *item = treeWidget->itemAt(point);
        if(item)
        {
            QMenu contextMenu;
            contextMenu.addAction(item->text(0).toStdString().c_str());
            contextMenu.exec(treeWidget->mapToGlobal(point));
        }
    }
}


void Window::on_tree_nodes_doubleClicked(const QModelIndex &index)
{

    POLL_DEBUG(std::cout, "double clicked a tree node, row: " << index.);
}


void Window::on_tree_nodes_activated(const QModelIndex &index)
{
    POLL_DEBUG(std::cout, "activated tree node, row: " << index.row());

}


void Window::showEvent(QShowEvent *)
{
     QMainWindow::show();
     QApplication::processEvents();
     POLL_DEBUG(std::cerr, "showing window...");

     scene_tree_fill();

}

void Window::on_menu_item_save_poll_scene_triggered()
{
    QString fileFormats = tr("Scene Files (*.obj)");
    QString openStartPoint = "/home/mike/Documents/";

    QString fileName = QFileDialog::getOpenFileName(NULL,
      tr("Save as Scene file..."), openStartPoint, fileFormats);

    POLL_DEBUG(std::cout, "save to scene file:");
    POLL_DEBUG(std::cout, fileName.toStdString());

}


void Window::on_menu_item_load_poll_scene_triggered()
{
    // adapted from http://qt-project.org/doc/qt-5/qfiledialog.html
    QString fileFormats = tr("Scene Files (*.obj)");
    QString openStartPoint = "/home/mike/Documents/";

    QString fileName = QFileDialog::getOpenFileName(NULL,
      tr("Select Scene file..."), openStartPoint, fileFormats);

    POLL_DEBUG(std::cout, "selected scene file:");
    POLL_DEBUG(std::cout, fileName.toStdString());

}
