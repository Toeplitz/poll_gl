#include "fragmic.h"
#include <iostream>

Fragmic fragmic("Demo 2", 1280, 720);


void keyboard_pressed_cb(SDL_Keysym *keysym)
{

  switch (keysym->sym) {
    default:
      break;
  }

}

int main() 
{
  Scene &scene = fragmic.scene_get();
  Assets &assets = scene.assets_get();
  Window &window = fragmic.window_get();
  window.keyboard_pressed_callback_set(keyboard_pressed_cb);

  Node *light_node = scene.node_create("Light");
  Light *light = light_node->light_create(assets);
  light->properties_type_set(LIGHT_DIRECTIONAL);
  light->properties_direction_set(glm::vec3(0, -1, 0));
  light->properties_set(glm::vec3(0.2, 0.2, 0.2), glm::vec3(1, 1, 1), glm::vec3(1.0, 1.0, 1.0));


  scene.model_load("data/crytek-sponza/", "sponza_with_spec.obj");
  // scene.model_load("data/crytek-sponza/", "banner.obj", MODEL_IMPORT_OPTIMIZED);

  // scene.scene_graph_print(false);
  scene.assets_get().print_all(scene.node_root_get());


  fragmic.run();
  fragmic.term();

  return true;
}
