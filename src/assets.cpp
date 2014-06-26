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
  std::cout << "Armatures: " << std::endl;
  for (auto &armature: armatures) {
    std::cout << "\t(" << &armature << ") ";
    std::cout << "node: '" << armature->find_toplevel_node()->name << "' ";
    std::cout << "bones: " << armature->bones_num_get() << std::endl;
  }

}


void Assets::material_add(std::unique_ptr<Material> &&material) 
{
  materials.push_back(std::move(material));
}


void Assets::material_print_all()
{
  std::cout << "Materials: " << std::endl;
  for (auto &material: materials) {
    auto &texture = material->texture;
    std::cout << "\t(" << &material << ") ";
    std::cout << "texture: " << texture->filename << std::endl;
  }

}


void Assets::mesh_add(std::unique_ptr<Mesh> &&mesh) 
{
  meshes.push_back(std::move(mesh));
}


void Assets::mesh_print_all()
{
  std::cout << "Meshes: " << std::endl;
  for (auto &mesh: meshes) {
    std::cout << "\t(" << &mesh<< ") " ;
    std::cout << "indices/vertices: " << mesh->indices.size();
    std::cout << " / " << mesh->vertices.size() << std::endl;

  }

}


void Assets::print_all()
{
  std::cout << "================= Current assets =================" << std::endl;
  armature_print_all();
  material_print_all();
  mesh_print_all();
  std::cout << "==================================================" << std::endl;
}

