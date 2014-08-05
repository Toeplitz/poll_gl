#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <memory>
#include "texture.h"


class Font
{
  private:
    unsigned char *buffer;
    Texture texture;

  public:
    Font();
    ~Font();

    void     bake();
    void     load(const std::string &filename);
    Texture &texture_get();
};



class Text
{
  private:
    Font *font;

  public:

    Text();
    ~Text();

    const std::vector<glm::vec4>  bake_coords(float x, float y, char *text);
    void                          font_set(Font *font);

};
