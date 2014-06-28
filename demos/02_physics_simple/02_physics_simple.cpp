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
  //Node &node = scene.load_model("data/", "scene_textured.dae");

  Node &node = scene.load_model("data/", "static_unit_box.dae");
  physics.collision_node_add(node, PHYSICS_COLLISION_BOX, true);
  physics.collision_node_callback_set(node, collision_cb);
  scene.scene_graph_print();

  fragmic.run();
  fragmic.term();

  return true;
}
