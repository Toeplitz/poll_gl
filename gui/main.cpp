#include "window.h"
#include <QApplication>
#include <QFile>
#include <iostream>
#include "utils.h"


int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  QFile file(":/qdarkstyle/style.qss");
  if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    a.setStyleSheet(file.readAll());
    file.close();
  }


  Window w;

  POLL_DEBUG(std::cout, "Poll data path: " << POLL_DATA_PATH);
  POLL_DEBUG(std::cout, "Poll shader path: " << POLL_SHADER_PATH);
  w.show();

  return a.exec();
}
