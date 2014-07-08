#include "fragmic.h"
#include <iostream>

Fragmic fragmic("Demo 4", 1280, 720);
  Physics_CharacterController *character;


void keyboard_pressed_cb(SDL_Keysym *keysym)
{
  Physics &physics = fragmic.physics_get();
  const float step = 0.25;

  switch (keysym->sym) {
    case SDLK_SPACE:
      physics.pause();
      break;
    case SDLK_d:
      physics.debug();
      break;
    case SDLK_j:
      character->jump();
      break;
    case SDLK_UP:
      character->setWalkDirection(btVector3(0.0, 0.0, step));
      break;
    case SDLK_DOWN:
      character->setWalkDirection(btVector3(0.0, 0.0, -step));
      break;
    case SDLK_RIGHT:
      character->setWalkDirection(btVector3(step, 0.0, 0.0));
      break;
    case SDLK_LEFT:
      character->setWalkDirection(btVector3(-step, 0.0, 0.0));
      break;
    default:
      break;
  }

}

void keyboard_released_cb(SDL_Keysym *keysym)
{

  switch (keysym->sym) {
    case SDLK_UP:
      character->setWalkDirection(btVector3(0.0, 0.0, 0));
      break;
    case SDLK_DOWN:
      character->setWalkDirection(btVector3(0.0, 0.0, 0));
      break;
    case SDLK_RIGHT:
      character->setWalkDirection(btVector3(0.0, 0.0, 0));
      break;
    case SDLK_LEFT:
      character->setWalkDirection(btVector3(0.0, 0.0, 0));
      character->setTurnAngle(-90);
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
  window.keyboard_released_callback_set(keyboard_released_cb);

  Node &room = scene.load_model("data/game_assets/", "Room.dae");
  physics.collision_node_add(room, PHYSICS_COLLISION_TRIANGLE_MESH, true, 0);

 // Node &box = scene.load_model("data/game_assets/", "cones.dae", 0);
 // physics.collision_node_add(box, PHYSICS_COLLISION_CONVEX_HULL, true, 1);
 
  //t.translate(panda, glm::vec3(30, 10, 0));
//  physics.collision_mesh_add(panda, "data/game_assets/characters/panda/", "PandaColl.dae");

  /*
  Node &bob = scene.load_model("data/bob/", "Bob_with_lamp.dae", 1);
  t.translate(bob, glm::vec3(-30, 10, 0));
  t.scale(bob, glm::vec3(3.5, 3.5, 3.5));
*/

 //Node &cylinder = scene.load_model("data/game_assets/characters/placeholder/", "cylinder.dae", 0);
 
  /*
  Node &box_root = scene.load_model("data/game_assets/", "box.dae", 0);
  Node *cube = scene.node_find(&box_root, "Cube");
  if (cube) {
    character = physics.character_controller_add(*cube);
  } else {
    std::cout << "Could not find node" << std::endl;
  }

  */
  {
    Node &panda = scene.load_model("data/game_assets/characters/panda/", "Panda.dae");
    Node &panda_collision = scene.load_model("data/game_assets/characters/panda/", "Panda_convex_hull.dae", false);
    Node *cube = scene.node_find(&panda, "Panda");
    Node *collision = scene.node_find(&panda_collision, "Panda_convex_hull");
    if (cube) {
      character = physics.character_controller_add(*cube, *collision);
      character->setJumpSpeed(20);
      character->setFallSpeed(10000);
    } else {
      std::cout << "Could not find node" << std::endl;
    }
  }


  scene.scene_graph_print();

  fragmic.run();
  fragmic.term();

  return true;
}
