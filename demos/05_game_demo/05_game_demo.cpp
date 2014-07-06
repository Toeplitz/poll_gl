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

  //Node &room = scene.load_model("data/game_assets/", "box.dae", 0);
  Node &room = scene.load_model("data/game_assets/", "Room.dae", 0);
  physics.collision_node_add(room, PHYSICS_COLLISION_TRIANGLE_MESH, true, 0);

  Node &box = scene.load_model("data/game_assets/", "cones.dae", 0);
  physics.collision_node_add(box, PHYSICS_COLLISION_CONVEX_HULL, true, 1);
 
  Node &panda = scene.load_model("data/game_assets/characters/panda/", "Panda.dae", 0);
  t.translate(panda, glm::vec3(30, 10, 0));
//  physics.collision_mesh_add(panda, "data/game_assets/characters/panda/", "PandaColl.dae");

  /*
  Node &bob = scene.load_model("data/bob/", "Bob_with_lamp.dae", 1);
  t.translate(bob, glm::vec3(-30, 10, 0));
  t.scale(bob, glm::vec3(3.5, 3.5, 3.5));
*/

 // Node &cylinder = scene.load_model("data/game_assets/characters/placeholder/", "cylinder.dae", 1);
 // physics.bullet_kinematic_character_controller_create(cylinder);

  scene.scene_graph_print();

  fragmic.run();
  fragmic.term();

  return true;
}
