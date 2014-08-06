#include "fragmic.h"
#include "common.h"
#include <iostream>

Fragmic fragmic("Showcase", 1280, 720);


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

  /*
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
  */

  Node &node = scene.model_load("demos/07_showcase/assets/", "scene.dae", MODEL_IMPORT_OPTIMIZED);

  scene.scene_graph_print(false);
  //scene.assets_get().print_all(scene.node_root_get());


  fragmic.run();
  fragmic.term();

  return true;
}
