#include "fragmic.h"
#include <iostream>

Fragmic fragmic("Skeletal demo", 1280, 720);


void keyboard_pressed_cb(SDL_Keysym *keysym)
{
  Physics &physics = fragmic.physics_get();

  switch (keysym->sym) {
    case SDLK_SPACE:
      physics.pause();
      break;
    case SDLK_o:
      physics.debug();
      break;
    default:
      break;
  }

}

int main() 
{
  Physics &physics = fragmic.physics_get();
  Scene &scene = fragmic.scene_get();
  Assets &assets= scene.assets_get();
  Window &window = fragmic.window_get();
  window.keyboard_pressed_callback_set(keyboard_pressed_cb);

  const float spot_offset = 25;

  {
    Node *light_node = scene.node_create("Light_spot_top");
    Light *light = light_node->light_create(assets);
    light->properties_type_set(LIGHT_SPOT);
    light->properties_position_set(glm::vec3(0, spot_offset, 0));
    light->properties_direction_set(glm::vec3(0, -1, 0));
  }

  {
    Node *light_node = scene.node_create("Light_spot_right");
    Light *light = light_node->light_create(assets);
    light->properties_type_set(LIGHT_SPOT);
    light->properties_position_set(glm::vec3(spot_offset, spot_offset, 0));
    light->properties_direction_set(glm::vec3(-1, 0, 0));
  }

  {
    Node *light_node = scene.node_create("Light_spot_left");
    Light *light = light_node->light_create(assets);
    light->properties_type_set(LIGHT_SPOT);
    light->properties_position_set(glm::vec3(-spot_offset, spot_offset, 0));
    light->properties_direction_set(glm::vec3(1, 0, 0));
  }
  {
    Node *light_node = scene.node_create("Light_spot_front");
    Light *light = light_node->light_create(assets);
    light->properties_type_set(LIGHT_SPOT);
    light->properties_position_set(glm::vec3(0, spot_offset, spot_offset));
    light->properties_direction_set(glm::vec3(0, 0, -1));
  }


  Node &plane = scene.model_load("data/game_assets/", "Room_no_slope.dae", MODEL_IMPORT_OPTIMIZED);
  physics.collision_shape_add(plane, PHYSICS_COLLISION_CONVEX_HULL, true, 1.f);
  physics.pause();

  Node &bob= scene.model_load("data/bob/", "Bob_with_lamp.dae", MODEL_IMPORT_OPTIMIZED);
  bob.scale(glm::vec3(25, 25, 25));
  bob.translate(glm::vec3(0, -1.25, 0));
  scene.scene_graph_print(true);

  fragmic.run();
  fragmic.term();

  return true;
}
