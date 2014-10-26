#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>
#include "texture.h"


using glm::vec3;
using glm::vec4;


struct Material_Properties
{
  glm::vec4 Ka;
  glm::vec4 Kd;
  glm::vec4 Ks;
  float shininess;
};


enum Cubemap_Type
{
  CUBEMAP_SKYBOX,
  CUBEMAP_REFLECTION,
  CUBEMAP_REFRACTION
};


struct Cubemap_Item
{
  GLenum target;
  Texture texture;
};


struct Cubemap
{
  Cubemap_Type type;
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
    std::unique_ptr<Texture> height;
    std::unique_ptr<Texture> alpha;
    std::unique_ptr<Texture> specular;
    std::unique_ptr<Cubemap> cubemap;

    Material_Properties properties;

    Material();
    ~Material();

    void color_set(const glm::vec3 ambient, const glm::vec3 diffuse, const glm::vec3 specular, const float shininess);
    void cubemap_type_set(const Cubemap_Type type);
    void cubemap_create(const Cubemap_Type type, const std::string &prefix, const std::string &front, const std::string &back,
        const std::string &top, const std::string &bottom, 
        const std::string &left, const std::string &right);
    void print(const int indent_level);
};

