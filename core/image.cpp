#include "image.h"
#include "utils.h"
#include <cstring>


bool Image::load(const std::string &filename)
{
  if(!image.load(QString(filename.c_str()))) {
    POLL_WARN(std::cerr, "Cannot load image: " << filename);
  }

  gl_image = QGLWidget::convertToGLFormat(image);

  width = image.width();
  height = image.height();

  POLL_DEBUG(std::cout, filename << " loaded, size: " << width << " x " << height);

  auto format = gl_image.format();

  /* Overview of Qt formats:
   * http://qt-project.org/doc/qt-4.8/qimage.html#Format-enum
   */
  switch (format) {
  case QImage::Format_ARGB32:
    break;
  default:
    POLL_WARN(std::cerr, "Format not confirmed, please be aware! Format: " << format);
    break;
  }

  return true;
}


void Image::data_copy(unsigned char *buf, const int width, const int height)
{
  /*
  if (data.size() > 0) {
    std::cout << "Error: image data already exists" << std::endl;
    return;
  }
  size_t size = width * height;
  data.assign(buf, buf + size);
  this->width = width;
  this->height = height;
  */

  POLL_ERROR(std::cerr, "THIS FUNCTION IS NOT IMPLEMENTED");
}


const unsigned char &Image::data_get() const
{
  return *gl_image.bits();
}


