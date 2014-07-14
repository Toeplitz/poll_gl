#include "material.h"
#include "utils.h"

/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Material::Material()
{
  diffuse.reset(nullptr);
  normal.reset(nullptr);
  specular.reset(nullptr);
  cubemap.reset(nullptr);
}


Material::~Material()
{
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Material::cubemap_create(const Cubemap_Type type, const std::string &prefix, const std::string &front, const std::string &back,
                              const std::string &top, const std::string &bottom, 
                              const std::string &left, const std::string &right)
{

  std::unique_ptr<Cubemap> cubemap_ptr(new Cubemap);
  cubemap_ptr->front.texture.image_load(prefix + front);
  cubemap_ptr->front.target = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
  cubemap_ptr->back.texture.image_load(prefix + back);
  cubemap_ptr->back.target = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
  cubemap_ptr->top.texture.image_load(prefix + top);
  cubemap_ptr->top.target = GL_TEXTURE_CUBE_MAP_POSITIVE_Y; 
  cubemap_ptr->bottom.texture.image_load(prefix + bottom);
  cubemap_ptr->bottom.target = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
  cubemap_ptr->left.texture.image_load(prefix + left);
  cubemap_ptr->left.target = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
  cubemap_ptr->right.texture.image_load(prefix + right);
  cubemap_ptr->right.target = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
  cubemap = std::move(cubemap_ptr);
  cubemap_type_set(type);
}


void Material::cubemap_type_set(const Cubemap_Type type)
{
  if (!cubemap) {
    std::cout << "Error: no cubemap created, cannot set type" << std::endl;
    return;
  }
  cubemap->type = type;
}


void Material::color_set(const glm::vec3 ambient, const glm::vec3 diffuse, const glm::vec3 specular, const float shininess)
{
  properties.Ka = glm::vec4(ambient, 1.f);
  properties.Kd = glm::vec4(diffuse, 1.f);
  properties.Ks = glm::vec4(specular, 1.f);
  properties.shininess = shininess;
}


void Material::print(const int indent_level)
{
  indent(std::cout, indent_level);
  if (diffuse) {
    std::cout << "\tdiffuse texture '" << diffuse->filename << "'" << std::endl;
  }
  if (normal) {
    std::cout << "\tnormal texture '" << normal->filename << "'" << std::endl;
  }
  if (specular) {
    std::cout << "\tspecular texture '" << specular->filename << "'" << std::endl;
  }

  std::cout << "\tshininess = " << properties.shininess << " ";
  std::cout << "Ka = ("<< properties.Ka.x << ", " << properties.Ka.y << ", " << properties.Ka.z << ") ";
  std::cout << "Kd = ("<< properties.Kd.x << ", " << properties.Kd.y << ", " << properties.Kd.z << ") ";
  std::cout << "Ks = ("<< properties.Ks.x << ", " << properties.Ks.y << ", " << properties.Ks.z << ")" << std::endl;
  std::cout << "ptr: " << this << std::endl;

}
