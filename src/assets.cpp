#include "assets.h"


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Assets::Assets() {
} 


Assets::~Assets() {
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/

void Assets::armature_add(std::unique_ptr<Armature> &&armature) 
{
  armatures.push_back(std::move(armature));
}


Armature_Unique_Ptr_List const &Assets::armature_get_all() const
{
  return armatures;
} 


void Assets::armature_print_all()
{

}


void Assets::material_add(std::unique_ptr<Material> &&material) 
{
  materials.push_back(std::move(material));
}


void Assets::material_print_all()
{

}


void Assets::mesh_add(std::unique_ptr<Mesh> &&mesh) 
{
  meshes.push_back(std::move(mesh));
}


void Assets::mesh_print_all()
{

}
