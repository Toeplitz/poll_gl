#include "assets.h"
#include "utils.h"
#include <memory>
#include <algorithm> 


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Assets::Assets() 
{
} 


Assets::~Assets() 
{
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


void Assets::armature_print_all() const
{
  std::cout << "Armatures: " << std::endl;
  for (auto &armature: armatures) {
    std::cout << "\t(" << &armature << ") ";
    std::cout << "node: '" << armature->find_toplevel_node()->name << "' ";
    std::cout << "bones: " << armature->bones_num_get() << std::endl;
  }

}


void Assets::camera_add(std::unique_ptr<Camera> &&camera)
{
  cameras.push_back(std::move(camera));
}


void Assets::camera_print_all(const Node &node) const
{
  std::cout << "\nCameras: " << std::endl;

  for (auto &camera: cameras) {
    std::cout << "\t(" << camera.get() << ")" << std::endl;
  }
}



void Assets::light_activate(Light *light)
{
  bool flag = false;
  std::vector<std::unique_ptr<Light>>::iterator found;

  for (auto it = inactive_lights.begin(); it != inactive_lights.end(); ++it) {
    if (it->get() == light) {
      active_lights.push_back(std::move(*it));
      found = it;
      flag = true;
      break;
    }
  }

  if (flag)
    inactive_lights.erase(found);
}


void Assets::light_active_add(std::unique_ptr<Light> &&light) 
{
  active_lights.push_back(std::move(light));
}


Light_Unique_Ptr_List const &Assets::light_active_get() const
{
  return active_lights;
} 


Light *Assets::light_ith_get(const int i)
{
  unsigned int total_count = active_lights.size() + inactive_lights.size();
  if (i < 0 || static_cast<unsigned int>(i) >= total_count) {
    return nullptr;
  }

  if (!active_lights.size()) {
    return inactive_lights[i].get();
  } else if (static_cast<unsigned int>(i) > active_lights.size() - 1) {
    return inactive_lights[i - active_lights.size()].get();
  } else {
    return active_lights[i].get();
  }
} 


void Assets::light_inactive_add(std::unique_ptr<Light> &&light) 
{
  inactive_lights.push_back(std::move(light));
}


bool Assets::light_is_active(Light *light)
{
  for (std::vector<std::unique_ptr<Light>>::iterator it = active_lights.begin();
      it != active_lights.end(); ++it) {
    if (it->get() == light) {
      return true;
    }
  }
  return false;
}


void Assets::light_deactivate(Light *light)
{
  bool flag = false;
  std::vector<std::unique_ptr<Light>>::iterator found;

  for (std::vector<std::unique_ptr<Light>>::iterator it = active_lights.begin();
      it != active_lights.end(); ++it) {
    if (it->get() == light) {
      inactive_lights.push_back(std::move(*it));
      found = it;
      flag = true;
      break;
    } 
  }

  if (flag)
    active_lights.erase(found);
}



void Assets::light_print_all(const Node &node) const
{
  std::cout << "\nLights: " << std::endl;

  std::cout << "\tactive: " << std::endl;
  int count = 0;
  for (auto &light: active_lights) {
    std::cout << "\t[" << count << "] (" << light.get() << ") ";

    if (light->volume_mesh_get()) {
      std::cout << "(volume mesh) ";
    }

    std::cout << std::endl;
    count++;
  }
  std::cout << "\tinactive: " << std::endl;
  for (auto &light: inactive_lights) {
    std::cout << "\t[" << count << "] (" << light.get() << ") ";

    if (light->volume_mesh_get()) {
      std::cout << "(volume mesh) ";
    }

    std::cout << std::endl;
    count++;
  }
}


void Assets::print_all(const Node &node) const
{
  std::cout << "======== Current assets (owned by engine) ========" << std::endl;
  armature_print_all();
  camera_print_all(node);
  light_print_all(node);
  manipulator_print_all(node);
  material_print_all(node);
  mesh_print_all(node);
  text_print_all(node);
  std::cout << "==================================================" << std::endl;
}


void Assets::manipulator_add(std::unique_ptr<Manipulator> &&manipulator) 
{
  manipulators.push_back(std::move(manipulator));
}


void Assets::manipulator_print_all(const Node &node) const
{
  std::cout << "\nManipulators: " << std::endl;

  for (auto &manipulator: manipulators) {
    std::cout << "\t(" << manipulator.get() << ")" << std::endl;
  }
}


void Assets::material_add(std::unique_ptr<Material> &&material) 
{
  materials.push_back(std::move(material));
}


void Assets::material_print_all(const Node &node) const
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


void Assets::physics_rigidbody_add(std::unique_ptr<Physics_Rigidbody> &&rigidbody)
{
  rigidbodies.push_back(std::move(rigidbody));
}


unsigned int Assets::material_node_lookup(const Material *material, const Node &node) const
{
  unsigned int count = 0;

  if (material == node.material) 
    count++;

  for (auto &child : node.children) {
    count += material_node_lookup(material, *child);
  }

  return count;
}


void Assets::mesh_add(std::unique_ptr<Mesh> &&mesh) 
{
  meshes.push_back(std::move(mesh));
}


unsigned int Assets::mesh_node_lookup(const Mesh *mesh, const Node &node) const
{
  unsigned int count = 0;

  if (mesh == node.mesh) 
    count++;

  for (auto &child : node.children) {
    count += mesh_node_lookup(mesh, *child);
  }

  return count;
}


void Assets::mesh_print_all(const Node &node) const
{
  std::cout << "\nMeshes: " << std::endl;
  for (auto &mesh: meshes) {
    unsigned int count = mesh_node_lookup(mesh.get(), node);
    std::cout << "\t(" << &mesh << ") (node count: " << count << ") ";
    std::cout << "indices/vertices: " << mesh->num_indices_get();
    std::cout << " / " << mesh->num_vertices_get() << std::endl;
  }

}


void Assets::text_add(std::unique_ptr<Text> &&text) 
{
  texts.push_back(std::move(text));
}


void Assets::text_print_all(const Node &node) const
{
  std::cout << "\nText: " << std::endl;

  for (auto &text: texts) {
    std::cout << "\t(" << text.get() << ")" << std::endl;
  }
}
