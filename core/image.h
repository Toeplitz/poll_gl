#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <QImage>
#include <QGLWidget>
#include <QString>


class Image {
  private:
    QImage image;
    QImage gl_image;

public:
  int width;
  int height;

  bool                 load(const std::string &filename);
  void                 data_copy(unsigned char *buf, const int width, const int height);
  const unsigned char &data_get() const;

};

