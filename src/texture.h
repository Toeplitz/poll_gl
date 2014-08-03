#pragma once

#include <iostream>
#include <memory>
#include <vector>

#include "gldefaults.h"
#include "image.h"

class Texture {
  private:

  public:
    std::unique_ptr<Image> image;
    std::string filename;
    GLuint gl_texture;

    Texture();
    ~Texture();

    bool    image_load(const std::string &filename);
    Image  &image_get();

};

