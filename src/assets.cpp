#include "assets.h"
#include "glcontext.h"
#include "utils.h"
#include "scene.h"
#include <memory>
#include <algorithm> 
#include <glm/gtx/string_cast.hpp>


/**************************************************/
/************* STOCK SHADERS CLASS ****************/
/**************************************************/

/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Stock_Shaders::init(Config &config, Scene &scene)
{
  GLcontext &glcontext = scene.glcontext_get();
  glcontext.uniform_buffers_create(config);

  screen_light.load("screen_light.v", "screen_light.f");
  glcontext.uniform_locations_lighting_init(screen_light);
  glcontext.uniform_buffers_block_bind(screen_light);

  screen_post_proc.load("screen_post_proc.v", "screen_post_proc.f");
  glcontext.uniform_locations_post_proc_init(screen_post_proc);
  glcontext.uniform_buffers_block_bind(screen_post_proc);

  world_basic_color.load("world_basic_color.v", "world_basic_color.f");
  glcontext.uniform_buffers_block_bind(world_basic_color);

  world_geometry.load("world_geometry.v", "world_geometry.f");
  glcontext.uniform_locations_geometry_init(world_geometry);
  glcontext.uniform_buffers_block_bind(world_geometry);

  world_light.load("world_light.v", "world_light.f");
  glcontext.uniform_locations_lighting_init(world_light);
  glcontext.uniform_buffers_block_bind(world_light);

  world_stencil.load("world_stencil.v", "world_stencil.f");
  glcontext.uniform_buffers_block_bind(world_stencil);

  world_physics_debug.load("world_physics_debug.v", "world_physics_debug.f");
  glcontext.uniform_buffers_block_bind(world_physics_debug);

  text.load("text.v", "text.f"); 
  glcontext.uniform_buffers_block_bind(text);
  glcontext.uniform_locations_text_init(text);
}


void Stock_Shaders::term()
{
  screen_post_proc.term();
  screen_light.term();
  text.term();
  world_basic_color.term();
  world_geometry.term();
  world_light.term();
  world_stencil.term();
  world_physics_debug.term();
}


/**************************************************/
/************** STOCK NODES CLASS *****************/
/**************************************************/

/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


 void Stock_Nodes::init(Scene &scene)
{
  GLcontext &glcontext = scene.glcontext_get();

  {
    node_symbol_cone = &scene.load(glcontext, "data/", "cone.dae", MODEL_IMPORT_OPTIMIZED | MODEL_IMPORT_NO_DRAW);
    node_symbol_cone->name_set("stock_cone");
    Mesh *mesh = node_symbol_cone->mesh_get();
    /* update the vertices since this is a blender export */
    mesh->positions_update(node_symbol_cone->transform_global_get());
    glcontext.vertex_buffers_mesh_create(node_symbol_cone->mesh_get());
  }

  {
    node_light_sphere = &scene.load(glcontext, "data/", "sphere.obj", MODEL_IMPORT_OPTIMIZED | MODEL_IMPORT_NO_DRAW);
    node_light_sphere->name_set("stock_sphere");
    Mesh *mesh = node_light_sphere->mesh_get();
    mesh->positions_update(node_light_sphere->transform_global_get());
    glcontext.vertex_buffers_mesh_create(node_light_sphere->mesh_get());
  }

  {
    node_symbol_pyramid = scene.node_create("stock_pyramid");
    Mesh *mesh = node_symbol_pyramid->mesh_create(scene);
    mesh->generate_pyramid(1.f);
    glcontext.vertex_buffers_mesh_create(mesh);
  }

  {
    node_screen_quad = scene.node_create("stock_screen_quad");
    Mesh *mesh = node_screen_quad->mesh_create(scene);
    mesh->generate_quad(1.f);
    glcontext.vertex_buffers_mesh_create(mesh);
  }

}


Node *Stock_Nodes::cone_get()
{
  return node_symbol_cone;
}


Mesh *Stock_Nodes::disk_get()
{
  return node_symbol_disk->mesh_get();
}


Mesh *Stock_Nodes::diamond_get()
{
  return node_symbol_diamond->mesh_get();;
}


Mesh *Stock_Nodes::pyramid_get()
{
  return node_symbol_pyramid->mesh_get();
}


Mesh *Stock_Nodes::screen_quad_get() 
{
  return node_screen_quad->mesh_get();
}


Mesh *Stock_Nodes::sphere_get()
{
  return node_light_sphere->mesh_get();
}


/**************************************************/
/***************** ASSETS CLASS *******************/
/**************************************************/

/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Assets::armature_add(std::unique_ptr<Armature> &&armature) 
{
  armatures.push_back(std::move(armature));
}


Armature_List const &Assets::armature_get_all() const
{
  return armatures;
} 


void Assets::armature_print_all() const
{
  std::cout << "Armatures: " << std::endl;
  for (auto &armature: armatures) {
    std::cout << "\t(" << &armature << ") ";
    std::cout << "node: '" << armature->find_toplevel_node()->name_get() << "' ";
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
    std::cout << "\t(" << camera.get() << ") pos: " << glm::to_string(camera->position_get()) << std::endl;
  }
}


void Assets::init(Config &config, Scene &scene)
{
  stock_nodes.init(scene);
  stock_shaders.init(config, scene);
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


Light_List const &Assets::light_active_get() const
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
    auto &prop = light->properties_get();
    std::cout << "\t[" << count << "] (" << light.get() << ") ";
    std::cout << "x, y, z = " << prop.position.x << ", " << prop.position.y << ", " << prop.position.z;
    std::cout << std::endl;
    count++;
  }
  std::cout << "\tinactive: " << std::endl;
  for (auto &light: inactive_lights) {
    auto &prop = light->properties_get();
    std::cout << "\t[" << count << "] (" << light.get() << ") ";
    std::cout << "x, y, z = " << prop.position.x << ", " << prop.position.y << ", " << prop.position.z;
    std::cout << std::endl;
    count++;
  }
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


Material_List const &Assets::material_get_all() const
{
  return materials;
}


void Assets::material_print_all(Node &node) 
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



unsigned int Assets::material_node_lookup(const Material *material, Node &node) 
{
  unsigned int count = 0;

  if (material == node.material_get()) 
    count++;

  for (auto &child : node.children_get()) {
    count += material_node_lookup(material, *child);
  }

  return count;
}


void Assets::mesh_add(std::unique_ptr<Mesh> &&mesh) 
{
  meshes.push_back(std::move(mesh));
}


Mesh_List const &Assets::mesh_get_all() const
{
  return meshes;
}


unsigned int Assets::mesh_node_lookup(const Mesh *mesh, Node &node) 
{
  unsigned int count = 0;

  if (mesh == node.mesh_get()) 
    count++;

  for (auto &child : node.children_get()) {
    count += mesh_node_lookup(mesh, *child);
  }

  return count;
}


void Assets::mesh_print_all(Node &node) 
{
  std::cout << "\nMeshes: " << std::endl;
  for (auto &mesh: meshes) {
    unsigned int count = mesh_node_lookup(mesh.get(), node);
    std::cout << "\t(" << &mesh << ") (node count: " << count << ") ";
    std::cout << "indices/vertices: " << mesh->num_indices_get();
    std::cout << " / " << mesh->num_vertices_get() << std::endl;
  }

}


void Assets::physics_rigidbody_add(std::unique_ptr<Physics_Rigidbody> &&rigidbody)
{
  rigidbodies.push_back(std::move(rigidbody));
}


void Assets::physics_rigidbody_print_all(Node &node) 
{
  std::cout << "\nPhysics rigidbody: " << std::endl;
  for (auto &rigidbody: rigidbodies) {
    Node *node_ptr = rigidbody->node_ptr_get();
    std::cout << "\t(" << &rigidbody << ") node_ptr: " << node_ptr->name_get() << " ";
    if (rigidbody->shape_get() == Physics_Rigidbody::BOX) {
      std::cout << " (box)";
    } else if (rigidbody->shape_get() == Physics_Rigidbody::SPHERE) {
      std::cout << " (sphere)";
    } else if (rigidbody->shape_get() == Physics_Rigidbody::CONVEX_HULL) {
      std::cout << " (convex hull)";
    } else if (rigidbody->shape_get() == Physics_Rigidbody::TRIANGLE_MESH) {
      std::cout << " (triangle mesh)";
    }

    if (rigidbody->type_get() == Physics_Rigidbody::DYNAMIC) {
      std::cout << " (dynamic)";
    } else if (rigidbody->type_get() == Physics_Rigidbody::KINEMATIC) {
      std::cout << " (kinematic)";
    }

    std::cout << " mass: " << rigidbody->mass_get();
    std::cout << std::endl;
  }

}

void Assets::print_all(Node &node)
{
  std::cout << "======== Current assets (owned by engine) ========" << std::endl;
  armature_print_all();
  camera_print_all(node);
  light_print_all(node);
  manipulator_print_all(node);
  material_print_all(node);
  mesh_print_all(node);
  physics_rigidbody_print_all(node);
  text_print_all(node);
  std::cout << "==================================================" << std::endl;
}


Stock_Nodes &Assets::stock_nodes_get()
{
  return stock_nodes;
}


Stock_Shaders &Assets::stock_shaders_get()
{
  return stock_shaders;
}


void Assets::text_add(std::unique_ptr<Text> &&text) 
{
  texts.push_back(std::move(text));
}


void Assets::text_print_all(const Node &node) const
{
  std::cout << "\nText: " << std::endl;

  for (auto &text: texts) {
    std::cout << "\t(" << text.get() << ")" << " string: '" << text.get()->string_get().substr(0, 100) << "'" << std::endl;
  }
}


void Assets::term(Scene &scene)
{
  GLcontext &glcontext = scene.glcontext_get();

  for (auto &mesh: mesh_get_all()) {
    glcontext.vertex_buffers_mesh_delete(mesh.get());
  }
  for (auto &material: material_get_all()) {
    glcontext.texture_materials_delete(material.get());
  }

  stock_shaders.term();

}
