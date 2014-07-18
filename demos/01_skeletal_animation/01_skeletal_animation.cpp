#include "fragmic.h"
#include <iostream>

Fragmic fragmic("Skeletal demo", 1280, 720);


void keyboard_pressed_cb(SDL_Keysym *keysym)
{
  Physics &physics = fragmic.physics_get();

  switch (keysym->sym) {
    case SDLK_SPACE:
      break;
    case SDLK_o:
      break;
    default:
      break;
  }

}

int main() 
{
  Scene &scene = fragmic.scene_get();
  Assets &assets= scene.assets_get();
  Window &window = fragmic.window_get();
  window.keyboard_pressed_callback_set(keyboard_pressed_cb);

  {
    Node *light_node = scene.node_create("Light");
    Light *light = light_node->light_create(assets);
    light->properties_type_set(LIGHT_POINT);
    light->properties_position_set(glm::vec3(5, 5, 0));
    light->properties_set(glm::vec3(0.5, 0.5, 0.5), glm::vec3(1, 1, 1), glm::vec3(1.0, 1.0, 1.0));
  }

  {
    Node *light_node = scene.node_create("Light");
    Light *light = light_node->light_create(assets);
    light->properties_type_set(LIGHT_POINT);
    light->properties_position_set(glm::vec3(-5, -5, 0));
    light->properties_set(glm::vec3(0.5, 0.5, 0.5), glm::vec3(1, 1, 1), glm::vec3(1.0, 1.0, 1.0));
  }

  Node &plane = scene.model_load("data/", "plane.dae");


  Node &bob= scene.model_load("data/bob/", "Bob_with_lamp.dae");
  bob.scale(glm::vec3(100, 100, 100));
  bob.translate(glm::vec3(0, 0, 0));

  scene.scene_graph_print(true);

  fragmic.run();
  fragmic.term();

  return true;
}
