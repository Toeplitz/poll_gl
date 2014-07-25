#include "fragmic.h"
#include "common.h"
#include <iostream>

Fragmic fragmic("Demo 2", 1280, 720);


int main() 
{
  Scene &scene = fragmic.scene_get();
  Assets &assets = scene.assets_get();

  {
    Node *camera_node = scene.node_camera_get();
    common_init(fragmic);
    common_fpcamera_use(camera_node);
    common_debug_use();
  }

  {
    Node *light_node = scene.node_create("Light");
    Light *light = light_node->light_create(assets);
    light->properties_type_set(LIGHT_DIRECTIONAL);
    light->properties_direction_set(glm::vec3(0, -1, 0));
    light->properties_set(glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.2, 0.2, 0.2), glm::vec3(.2, .2, .2));
  }
  {
    Node *light_node = scene.node_create("Light");
    Light *light = light_node->light_create(assets);
    light->properties_type_set(LIGHT_POINT);
    light->properties_position_set(glm::vec3(5, 5, -0.2));
    light->properties_set(glm::vec3(1, 1, 1), glm::vec3(1, 1, 1), glm::vec3(1.0, 1.0, 1.0));
  }

  Node &root = scene.node_root_get();
  Node &sponza = scene.model_load("data/crytek-sponza/", "sponza_with_spec.obj", MODEL_IMPORT_OPTIMIZED);
  root.scale(glm::vec3(0.01, 0.01, 0.01));
  // scene.model_load("data/crytek-sponza/", "banner.obj", MODEL_IMPORT_OPTIMIZED);

  scene.scene_graph_print(false);
  //scene.assets_get().print_all(scene.node_root_get());


  fragmic.run();
  fragmic.term();

  return true;
}
