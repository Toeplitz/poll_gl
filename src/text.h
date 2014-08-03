#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <memory>
#include "texture.h"


class Text
{
  private:
    Texture texture;

    unsigned char *buffer;

  public:

    Text();
    ~Text();

    void                  font_read(const std::string &font_file);
    void                  font_bitmap_bake();
    Texture              &font_texture_get();
    const glm::vec4      &font_coordinates_get();

};
