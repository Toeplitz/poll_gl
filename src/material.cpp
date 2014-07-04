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
}


Material::~Material()
{
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/

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

  std::cout << "\tshininess = " << material_block.shininess << " ";
  std::cout << "Ka = ("<< material_block.Ka.x << ", " << material_block.Ka.y << ", " << material_block.Ka.z << ") ";
  std::cout << "Kd = ("<< material_block.Kd.x << ", " << material_block.Kd.y << ", " << material_block.Kd.z << ") ";
  std::cout << "Ks = ("<< material_block.Ks.x << ", " << material_block.Ks.y << ", " << material_block.Ks.z << ")" << std::endl;

}
