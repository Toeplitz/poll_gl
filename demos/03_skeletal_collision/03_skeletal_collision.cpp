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

  Node &bob_node = scene.load_model("data/game_assets/characters/panda/", "Panda.dae");
 // physics.collision_node_add(bob_node, PHYSICS_COLLISION_CONVEX_HULL, true, 1.f);


  scene.scene_graph_print();

  fragmic.run();
  fragmic.term();

  return true;
}
