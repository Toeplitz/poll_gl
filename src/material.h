#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>
#include "texture.h"

struct Material_Properties
{
  glm::vec4 Ka;
  glm::vec4 Kd;
  glm::vec4 Ks;
  float shininess;
};


struct Cubemap_Item
{
  GLenum target;
  Texture texture;
};


struct Cubemap
{
  GLuint gl_texture;
  Cubemap_Item front;
  Cubemap_Item back;
  Cubemap_Item top;
  Cubemap_Item bottom;
  Cubemap_Item left;
  Cubemap_Item right;
};


class Material 
{
  public:
    std::unique_ptr<Texture> diffuse;
    std::unique_ptr<Texture> normal;
    std::unique_ptr<Texture> specular;
    std::unique_ptr<Cubemap> cubemap;

    Material_Properties material_block;

    Material();
    ~Material();

    void color_set(const glm::vec3 ambient, const glm::vec3 diffuse, const glm::vec3 specular, const float shininess);
    void cubemap_create(const std::string &prefix, const std::string &front, const std::string &back,
        const std::string &top, const std::string &bottom, 
        const std::string &left, const std::string &right);
    void print(const int indent_level);
};

