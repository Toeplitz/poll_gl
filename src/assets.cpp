#include "assets.h"
#include "utils.h"


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


void Assets::light_add(std::unique_ptr<Light> &&light) 
{
  lights.push_back(std::move(light));
}


void Assets::light_print_all(const Node &node)
{
  std::cout << "\nLights: " << std::endl;
  for (auto &light: lights) {
    std::cout << "\t(" << &light << std::endl;
  }
}


void Assets::material_add(std::unique_ptr<Material> &&material) 
{
  materials.push_back(std::move(material));
}


unsigned int Assets::material_node_lookup(const Material *material, const Node &node)
{
  unsigned int count = 0;

  if (material == node.material) 
    count++;

  for (auto &child : node.children) {
    count += material_node_lookup(material, *child);
  }

  return count;
}


unsigned int Assets::mesh_node_lookup(const Mesh *mesh, const Node &node)
{
  unsigned int count = 0;

  if (mesh == node.mesh) 
    count++;

  for (auto &child : node.children) {
    count += mesh_node_lookup(mesh, *child);
  }

  return count;
}


void Assets::material_print_all(const Node &node)
{
  std::cout << "Materials: " << std::endl;
  for (auto &material: materials) {
    unsigned int count = material_node_lookup(material.get(), node);
    auto &diffuse = material->diffuse;
    auto &normal = material->normal;
    auto &specular = material->specular;
    std::cout << "\t(" << &material << ") (node count: " << count << ") ";
    if (diffuse || normal || specular ) {
      std::cout << "texture(s): ";;
      
      if (diffuse) 
        std::cout << "diffuse: '" << basename(diffuse->filename) << "'";
      if (normal) 
        std::cout << " normal: '" << basename(normal->filename) << "'";
      if (specular) 
        std::cout << " specular: '" << basename(specular->filename) << "'";

      std::cout << std::endl;
    } else if (material->cubemap) {
      std::cout << "cubemap:  " << material->cubemap.get() << std::endl;
    }  else {
      std::cout << "colors only" << std::endl;
    }
  }

}


void Assets::mesh_add(std::unique_ptr<Mesh> &&mesh) 
{
  meshes.push_back(std::move(mesh));
}


void Assets::mesh_print_all(const Node &node)
{
  std::cout << "\nMeshes: " << std::endl;
  for (auto &mesh: meshes) {
    unsigned int count = mesh_node_lookup(mesh.get(), node);
    std::cout << "\t(" << &mesh << ") (node count: " << count << ") ";
    std::cout << "indices/vertices: " << mesh->num_indices_get();
    std::cout << " / " << mesh->num_vertices_get() << std::endl;
  }

}


void Assets::print_all(const Node &node)
{
  std::cout << "======== Current assets (owned by engine) ========" << std::endl;
  armature_print_all();
  material_print_all(node);
  mesh_print_all(node);
  light_print_all(node);
  std::cout << "==================================================" << std::endl;
}

