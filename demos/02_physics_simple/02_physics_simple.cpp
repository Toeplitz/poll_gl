#include "poll.h"
#include "poll_plugin.h"
#include "plugin_debug.h"
#include "plugin_light_tool.h"
#include "plugin_node_tool.h"
#include "plugin_firstperson_cam.h"
#include <iostream>
#include <memory>

using glm::vec3;


int main() 
{
  Poll poll;

  Scene &scene = poll.scene_get();
  Physics &physics = scene.physics_get();

  Node *camera_node = scene.node_camera_get();

  auto plugin_debug = std::unique_ptr<Plugin_Debug>(new Plugin_Debug(poll.console_get(), scene));
  auto plugin_node_tool = std::unique_ptr<Plugin_Node_Tool>(new Plugin_Node_Tool(poll.console_get(), scene, 0.7f));
  auto plugin_firstperson_camera = std::unique_ptr<Plugin_Firstperson_Camera>(new Plugin_Firstperson_Camera(poll.console_get(), scene, camera_node));
  poll.plugin_add(*plugin_debug);
  poll.plugin_add(*plugin_node_tool);
  poll.plugin_add(*plugin_firstperson_camera);

  {
    Node &root = scene.node_root_get();
    root.scale(scene, vec3(0.5, 0.5, 0.5));
  }


  Node &node = scene.load("data/", "orientation.dae", MODEL_IMPORT_DEFAULT | MODEL_IMPORT_BLENDER_FIX);
  Node &suzanne_center = *scene.node_find(&node, "Suzanne_center");
  suzanne_center.scale(scene, vec3(1, 1, 1));

  auto suzanne_center_shape = std::unique_ptr<Physics_Convex_Hull_Shape>(new Physics_Convex_Hull_Shape(suzanne_center));
  Physics_Rigidbody *suzanne_center_rigidbody = suzanne_center.physics_rigidbody_create(scene);
  suzanne_center_rigidbody->create(physics, *suzanne_center_shape, Physics_Rigidbody::KINEMATIC, 1);

  Node &suzanne_translated = *scene.node_find(&node, "Suzanne_translated");
  auto suzanne_translated_shape = std::unique_ptr<Physics_Triangle_Mesh_Shape>(new Physics_Triangle_Mesh_Shape(suzanne_translated));
  Physics_Rigidbody *suzeanne_translated_rigidbody = suzanne_translated.physics_rigidbody_create(scene);
  suzeanne_translated_rigidbody->create(physics, *suzanne_translated_shape, Physics_Rigidbody::KINEMATIC, 1);
  suzanne_translated.scale(scene, vec3(3, 3, 3));

  Node &sphere = *scene.node_find(&node, "Sphere_ms");
  glm::vec3 v = vec3(1.f, 1.f, 1.f);
  auto sphere_shape = std::unique_ptr<Physics_Box_Shape>(new Physics_Box_Shape(v));
  Physics_Rigidbody *sphere_rigidbody = sphere.physics_rigidbody_create(scene);
  sphere_rigidbody->create(physics, *sphere_shape, Physics_Rigidbody::KINEMATIC, 1);


#if 0
  {
   // Node &panda = scene.load("data/game_assets/characters/panda/", "PandaSingle.dae", MODEL_IMPORT_OPTIMIZED);
    Node &panda = scene.load("data/game_assets/characters/panda/", "PandaSingle.dae", MODEL_IMPORT_OPTIMIZED | MODEL_IMPORT_BLENDER_FIX);
    //Node &panda = scene.load("data/game_assets/characters/panda/", "panda.fbx", MODEL_IMPORT_OPTIMIZED);
    panda.rotate(scene, (float) -M_PI / 4.f, glm::vec3(1, 0, 0));
    panda.translate(scene, vec3(3, 4, 4));
    panda.scale(scene, vec3(0.5, 0.5, 0.5));
    //    panda.translate(scene, vec3(0, 0, -4));
  }

#endif

  Node &bob = scene.load("data/bob/", "Bob_with_lamp.dae", MODEL_IMPORT_DEFAULT);
  bob.rotate(scene, (float) M_PI/ 2.f, vec3(0, 1, 0));
  bob.translate(scene, vec3(0, 0.5, 5));
  bob.scale(scene, vec3(1.2, 1.2, 1.2));
  {
    Node *node = scene.node_create("Light_Directionl_Global");
    Light *light = node->light_create(scene, Light::DIRECTIONAL, Light::GLOBAL);
    node->translate(scene, vec3(0, 40, 0));
    light->properties_direction_set(vec3(0, -1, 0.5));
    light->properties_color_set(vec3(1., 1., 1.));
  }

  {
    Node *node = scene.node_create("Light_Directionl_Global");
    Light *light = node->light_create(scene, Light::DIRECTIONAL, Light::GLOBAL);
    node->translate(scene, vec3(0, 40, 0));
    light->properties_direction_set(vec3(0, -1, -0.5));
    light->properties_color_set(vec3(1., 1., 1.));
  }

  //physics.pause();

  poll.run();
  poll.term();

  return true;
}
