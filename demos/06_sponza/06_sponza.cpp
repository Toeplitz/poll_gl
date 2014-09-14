#include "poll.h"
#include "plugin_debug.h"
#include "plugin_firstperson_cam.h"
#include <iostream>

Poll poll;


int main() 
{
  Scene &scene = poll.scene_get();

  Node *camera_node = scene.node_camera_get();
  auto firstperson_camera = std::unique_ptr<Plugin_Firstperson_Camera>(new Plugin_Firstperson_Camera(poll.console_get(), scene, camera_node));
  poll.plugin_add(*firstperson_camera);

  auto debug = std::unique_ptr<Plugin_Debug>(new Plugin_Debug(poll.console_get(), scene));
  poll.plugin_add(*debug);

  Node &root = scene.node_root_get();
  Node &sponza = scene.load("data/crytek-sponza/", "sponza_with_spec.obj", MODEL_IMPORT_OPTIMIZED);
  sponza.scale(scene, glm::vec3(0.1, 0.1, 0.1));
  // scene.load("data/crytek-sponza/", "banner.obj", MODEL_IMPORT_OPTIMIZED);

  {
    Node *node = scene.node_create("Light_Directionl_Global");
    Light *light = node->light_create(scene, Light::DIRECTIONAL, Light::GLOBAL);
    node->translate(scene, glm::vec3(0, 40, 0));
    light->properties_direction_set(glm::vec3(0, -1, -1));
    light->properties_color_set(glm::vec3(0.5, 0.5, 0.5));
  }

  poll.run();
  poll.term();

  return true;
}
