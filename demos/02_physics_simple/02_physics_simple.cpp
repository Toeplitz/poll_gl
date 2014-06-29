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
    case SDLK_d:
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

  Node &box_node= scene.load_model("data/", "box_translated_scaled.dae");
  physics.collision_node_add(box_node, PHYSICS_COLLISION_BOX, true, 1.f);
  physics.collision_node_callback_set(box_node, collision_cb);

  Node &sphere_node = scene.load_model("data/", "sphere_translated_scaled.dae");
  physics.collision_node_add(sphere_node, PHYSICS_COLLISION_SPHERE, true, 1.f);
  physics.collision_node_callback_set(sphere_node, collision_cb);

  Node &base_node= scene.load_model("data/", "base.dae");
  physics.collision_node_add(base_node, PHYSICS_COLLISION_BOX, true, 0);

  scene.scene_graph_print();

  fragmic.run();
  fragmic.term();

  return true;
}
