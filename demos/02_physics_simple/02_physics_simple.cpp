#include "fragmic.h"
#include <iostream>

Fragmic fragmic("Demo 2", 1280, 720);

void collision_cb(int i)
{
  std::cout << "Detected collision" << std::endl;
}


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
  Scene &scene = fragmic.scene_get();
  Physics &physics = fragmic.physics_get();
  Window &window = fragmic.window_get();
  window.keyboard_pressed_callback_set(keyboard_pressed_cb);

  Node &box_node_rh = scene.model_load("data/", "box_translated_scaled.dae", MODEL_IMPORT_OPTIMIZED);
  physics.collision_node_add(box_node_rh, PHYSICS_COLLISION_BOX, true, 1.f);

  Node &sphere_node = scene.model_load("data/", "sphere_translated_scaled.dae", MODEL_IMPORT_OPTIMIZED);
  physics.collision_node_add(sphere_node, PHYSICS_COLLISION_SPHERE, true, 1.f);

  Node &monkey_node = scene.model_load("data/", "convex_hull.dae", MODEL_IMPORT_OPTIMIZED);
  physics.collision_node_add(monkey_node, PHYSICS_COLLISION_CONVEX_HULL, true, 1.f);

  Node &base_node = scene.model_load("data/", "base.dae", MODEL_IMPORT_OPTIMIZED);
  physics.collision_node_add(base_node, PHYSICS_COLLISION_BOX, true, 0);

  scene.scene_graph_print(true);

  fragmic.run();
  fragmic.term();

  return true;
}
