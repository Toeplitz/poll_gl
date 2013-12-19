#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>
#include <memory>
#include <vector>

#include "gldefaults.h"
#include "image.h"

class Texture {
private:

public:
  GLuint textureId;

  std::unique_ptr < Image > image;
  std::string filename;

  Texture();
  ~Texture();
  bool loadImage(const std::string & _filename);
  void upload();


};

#endif
