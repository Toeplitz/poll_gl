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

  Node &crate =scene.load_model("data/normal_map/", "crate.dae", 0);
  physics.collision_node_add(crate, PHYSICS_COLLISION_CONVEX_HULL, true, 1);

  Node &box = scene.load_model("data/normal_map/", "box_simple.dae", 0);
  physics.collision_node_add(box, PHYSICS_COLLISION_CONVEX_HULL, true, 1);

  Node &floor = scene.load_model("data/normal_map/", "wood_floor.dae", 0);
  physics.collision_node_add(floor, PHYSICS_COLLISION_CONVEX_HULL, true, 0);

  Node &bob = scene.load_model("data/bob/", "Bob_with_lamp.dae", 1);
  t.translate(bob, glm::vec3(2.5, 2.5, 4));
  t.scale(bob, glm::vec3(0.3, 0.3, 0.3));

  scene.scene_graph_print();

  fragmic.run();
  fragmic.term();

  return true;
}
