#include "window.h"
#include "ui_window.h"
#include "utils.h"

#include <QTreeWidgetItem>
#include <QFileDialog>
#include <QMenu>



Fullscreen_Dialog::Fullscreen_Dialog(QWidget *parent)
{
      /*
  dlg = new QDialog(this);
  dlg_layout = new QHBoxLayout(dlg);
  dlg_layout->setContentsMargins(0, 0, 0, 0);
  dlg->setLayout(dlg_layout);
  dlg->showFullScreen();
  dlg->hide();

      */
  /*
  bool r = connect(dlg, SIGNAL(accepted()), this, SLOT(gl_show_fullscreen()));
  assert(r);

  r = connect(dlg, SIGNAL(rejected()), this, SLOT(gl_show_normal()));
  assert(r);
  */
}


Fullscreen_Dialog::~Fullscreen_Dialog()
{
 // if (dlg_layout) delete dlg_layout;
}


void Fullscreen_Dialog::gl_show_fullscreen()
{
  Window *win = (Window *) qApp->activeWindow();
  QGLWidget *gl_widget = win->findChild<QGLWidget *>("widget");
  dlg_layout = new QHBoxLayout(this);
  dlg_layout->setContentsMargins(0, 0, 0, 0);
  dlg_layout->addWidget(gl_widget);
  this->setLayout(dlg_layout);
  this->showFullScreen();
  this->show();
  this->grabKeyboard();
  this->grabMouse();
  this->activateWindow();
  this->raise();
  this->setFocus();
  this->setAttribute( Qt::WA_DeleteOnClose, true );
}


void Fullscreen_Dialog::gl_show_normal()
{
  Window *win = (Window *) qApp->activeWindow();
  QHBoxLayout *gl_box = win->findChild<QHBoxLayout *>("gl_hbox");
  QGLWidget *gl_widget = this->findChild<QGLWidget *>("widget");
  gl_box->layout()->addWidget(gl_widget);
  win->grabKeyboard();
  win->grabMouse();
  win->activateWindow();
  win->raise();
  win->setFocus();

  POLL_DEBUG(std::cout, "trying to close widget");
  if (this->close()) {
    POLL_DEBUG(std::cout, "success");
  }
}


void Fullscreen_Dialog::keyPressEvent(QKeyEvent *event)
{
  POLL_DEBUG(std::cout, "key pressed on fullscreen dialog");
  gl_show_normal();
}


Window::Window(QWidget *parent):
        QMainWindow(parent), ui(new Ui::window), fullscreen_dialog(this)
{
  ui->setupUi(this);
  fullMode = false;
  backupCentral = NULL;
  windowParent = parent;
}


Window::~Window()
{
  delete ui;
}

void Window::dataExchange()
{
    GLwidget *gl_widget = findChild<GLwidget *>("widget");
    QTreeWidget *treeWidget = findChild<QTreeWidget *>();

    treeWidget ->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(treeWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)));
    treeWidget->clear();

    Scene& widgetScene = gl_widget->getScene();
    Node &root = widgetScene.node_root_get();

    for (auto &child : root.children_get()) {
        node_recursive_load_tree(*child, treeWidget, NULL);
    }
}


void Window::node_recursive_load_tree(Node& node, QTreeWidget* tree, QTreeWidgetItem* treeItemIn)
{
    QTreeWidgetItem* treeItem = new QTreeWidgetItem();

    treeItem->setText(0, node.name_get().c_str());
    if(treeItemIn)
        treeItemIn->addChild(treeItem);
    else
        tree->addTopLevelItem(treeItem);

    Armature *armature = node.armature_get();
    Mesh *mesh = node.mesh_get();
    Material *material = node.material_get();

    if (armature) {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, "armature");
        treeItem->addChild(item);
    }

    if (mesh) {
      if (!(node.import_options & MODEL_IMPORT_NO_DRAW)) {
          QTreeWidgetItem *item = new QTreeWidgetItem();
          item->setText(0, "mesh - no draw");
          treeItem->addChild(item);
      }
      if (!(node.import_options & MODEL_IMPORT_NO_UPLOAD)) {
          QTreeWidgetItem *item = new QTreeWidgetItem();
          item->setText(0, "mesh - no upload");
          treeItem->addChild(item);
      }
    }

    if (material) {
      if (!(node.import_options & MODEL_IMPORT_NO_UPLOAD)) {
          QTreeWidgetItem *item = new QTreeWidgetItem();
          item->setText(0, "model - no upload");
          treeItem->addChild(item);
      }
    }


    for (auto &child : node.children_get()) {
      node_recursive_load_tree(*child, tree, treeItem);
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

  /*
  Window *win = (Window *) qApp->activeWindow();
  QHBoxLayout *gl_box = win->findChild<QHBoxLayout *>("gl_hbox");
  QGLWidget *gl_widget = win->findChild<QGLWidget *>("widget");

  QDialog *dlg = new QDialog(this);
  QHBoxLayout *dlg_layout = new QHBoxLayout(dlg);
  dlg_layout->setContentsMargins(0, 0, 0, 0);
  dlg_layout->addWidget(gl_widget);
  dlg->setLayout(dlg_layout);
  dlg->showFullScreen();

  bool r = connect(dlg, SIGNAL(accepted()), this, SLOT(showGlNormal()));
  assert(r);
  */
  //r = connect(dlg, SIGNAL(accepted()), this, SLOT(showGlNormal()));
  //assert(r);
}


void Window::showGlNormal()
{
  /*
  Window *win = (Window *) qApp->activeWindow();
  QHBoxLayout *gl_box = win->findChild<QHBoxLayout *>("gl_hbox");
  QGLWidget *gl_widget = win->findChild<QGLWidget *>("widget");
  gl_box->layout()->addWidget(gl_widget);
  */
}


void Window::on_menu_item_exit_triggered()
{
  exit(-1);
}


void Window::on_menu_item_new_scene_triggered()
{

}


void Window::on_menu_item_fullscreen_triggered()
{
  fullscreen_dialog.gl_show_fullscreen();
}

void Window::on_actionLoad_poll_scene_triggered()
{
    // adapted from http://qt-project.org/doc/qt-5/qfiledialog.html

    QString fileFormats = tr("Scene Files (*.obj)");
    QString openStartPoint = "/home/mike/Documents/";

    QString fileName = QFileDialog::getOpenFileName(NULL,
        tr("Select Scene file..."), openStartPoint, fileFormats);

    POLL_DEBUG(std::cout, "selected scene file:");
    POLL_DEBUG(std::cout, fileName.toStdString());
}

void Window::on_actionSave_poll_scene_triggered()
{
    // adapted from http://qt-project.org/doc/qt-5/qfiledialog.html

    QString fileFormats = tr("Scene Files (*.obj)");
    QString openStartPoint = "/home/mike/Documents/";

    QString fileName = QFileDialog::getOpenFileName(NULL,
        tr("Save as Scene file..."), openStartPoint, fileFormats);

    POLL_DEBUG(std::cout, "save to scene file:");
    POLL_DEBUG(std::cout, fileName.toStdString());
}

void Window::on_updateTree_clicked()
{
    dataExchange();
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
