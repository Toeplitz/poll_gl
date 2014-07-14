#include "fragmic.h"
#include <iostream>

Fragmic fragmic("Demo 4", 1280, 720);


void keyboard_pressed_cb(SDL_Keysym *keysym)
{
  Physics &physics = fragmic.physics_get();

  switch (keysym->sym) {
    case SDLK_SPACE:
      physics.pause();
      break;
    case SDLK_d:
      physics.debug();
      break;
    default:
      break;
  }

}

int main() 
{
  Transform t;
  Scene &scene = fragmic.scene_get();
  Window &window = fragmic.window_get();
  Physics &physics = fragmic.physics_get();

  window.keyboard_pressed_callback_set(keyboard_pressed_cb);

  Node &crate =scene.model_load("data/normal_map/", "crate.dae", MODEL_IMPORT_OPTIMIZED);
  physics.collision_node_add(crate, PHYSICS_COLLISION_TRIANGLE_MESH, true, 1);

  Node &box = scene.model_load("data/normal_map/", "box_simple.dae", MODEL_IMPORT_OPTIMIZED);
  physics.collision_node_add(box, PHYSICS_COLLISION_CONVEX_HULL, true, 1);

  Node &floor = scene.model_load("data/normal_map/", "wood_floor.dae", MODEL_IMPORT_OPTIMIZED);
  physics.collision_node_add(floor, PHYSICS_COLLISION_CONVEX_HULL, true, 0);

  scene.scene_graph_print(true);
  scene.assets_get().print_all(scene.node_root_get());

  fragmic.run();
  fragmic.term();

  return true;
}
