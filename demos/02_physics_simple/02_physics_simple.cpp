#include "poll.h"
#include "poll_plugin.h"
#include "plugin_debug.h"
#include "plugin_light_tool.h"
#include "plugin_firstperson_cam.h"
#include <iostream>
#include <memory>



int main() 
{
  Poll poll;

  Scene &scene = poll.scene_get();
  Assets &assets = scene.assets_get();
  Physics &physics = scene.physics_get();

  Node *camera_node = scene.node_camera_get();

  auto plugin_debug = std::unique_ptr<Plugin_Debug>(new Plugin_Debug(poll.console_get(), scene));
  auto plugin_light_tool = std::unique_ptr<Plugin_Light_Tool>(new Plugin_Light_Tool(poll.console_get(), scene));
  auto plugin_firstperson_camera = std::unique_ptr<Plugin_Firstperson_Camera>(new Plugin_Firstperson_Camera(poll.console_get(), scene, camera_node));
  poll.plugin_add(*plugin_debug);
  poll.plugin_add(*plugin_light_tool);
  poll.plugin_add(*plugin_firstperson_camera);


  Node &node = scene.load("data/", "orientation.dae", MODEL_IMPORT_DEFAULT | MODEL_IMPORT_BLENDER_FIX);

  {
    Node &suzanne_translated = *scene.node_find(&node, "Suzanne_center");
    suzanne_translated.translate(scene, glm::vec3(0, 0, 4));
  }

  /*
  {
    Node &sphere = *scene.node_create("sphere");
    Mesh &mesh = *sphere.mesh_create(scene);
    sphere.mesh_set(assets.stock_nodes_get().sphere_get()->mesh_get());
    scene.mesh_nodes_add(sphere);
    sphere.translate(scene, glm::vec3(0, 0, 4));
  }
  */

 /*
 Node &panda = scene.load("data/game_assets/characters/panda/", "PandaSingle.dae", MODEL_IMPORT_OPTIMIZED);
 panda.translate(scene, glm::vec3(0, 0, 4));
 panda.scale(scene, glm::vec3(0.5, 0.5, 0.5));

 Node &zombie = scene.load("data/zombie/", "new_thin_zombie.dae", MODEL_IMPORT_OPTIMIZED);
 zombie.rotate(scene, (float) M_PI / 4, glm::vec3(1, 0, 0));
 zombie.translate(scene, glm::vec3(0, 0, 0));
 */
 Node &bob = scene.load("data/bob/", "Bob_with_lamp.dae", MODEL_IMPORT_DEFAULT | MODEL_IMPORT_BLENDER_FIX);
 bob.scale(scene, glm::vec3(3, 3, 3));
 bob.translate(scene, glm::vec3(0, 10, 0));

  /*
  {
    Node *node = scene.node_create("Light_Directionl_Global");
    Light *light = node->light_create(scene, Light::DIRECTIONAL, Light::GLOBAL);
    node->translate(scene, glm::vec3(0, 40, 0));
    light->properties_direction_set(glm::vec3(0, -1, 0.5));
    light->properties_color_set(glm::vec3(1., 1., 1.));
  }
  */

  {
    Node *node = scene.node_create("Light_Directionl_Global");
    Light *light = node->light_create(scene, Light::DIRECTIONAL, Light::GLOBAL);
    node->translate(scene, glm::vec3(0, 40, 0));
    light->properties_direction_set(glm::vec3(0, -1, -0.5));
    light->properties_color_set(glm::vec3(1., 1., 1.));
  }

  physics.pause();

  poll.run();
  poll.term();

  return true;
}
