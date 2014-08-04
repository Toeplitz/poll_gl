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

    const std::vector<glm::vec4>   bake_coords(float x, float y, char *text);
    void                           font_read(const std::string &font_file);
    void                           font_bitmap_bake();
    Texture                       &texture_get();

};
