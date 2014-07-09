#include "fragmic.h"
#include "physics.h"
#include <iostream>

Fragmic fragmic("Demo 4", 1280, 720);
Physics_CharacterController *character;


unsigned int direction = 0;


void keyboard_pressed_cb(SDL_Keysym *keysym)
{
  Physics &physics = fragmic.physics_get();

  switch (keysym->sym) {
    case SDLK_SPACE:
      //character->setVelocityForTimeInterval(btVector3(0, -10, 0), 4);
      character->jump();
      break;
    case SDLK_d:
      physics.debug();
      break;
    case SDLK_f:
      physics.pause();
      break;
    case SDLK_UP:
      direction |= PHYSICS_DIRECTION_FORWARD;
      break;
    case SDLK_DOWN:
      direction |= PHYSICS_DIRECTION_BACK;
      break;
    case SDLK_RIGHT:
      direction |= PHYSICS_DIRECTION_RIGHT;
      break;
    case SDLK_LEFT:
      direction |= PHYSICS_DIRECTION_LEFT;
      break;
    case SDLK_1:
      direction |= PHYSICS_DIRECTION_STRAFE_LEFT;
      break;
    case SDLK_2:
      direction |= PHYSICS_DIRECTION_STRAFE_RIGHT;
      break;
    default:
      break;
  }

  character->move(static_cast<Physics_Direction>(direction));
}

void keyboard_released_cb(SDL_Keysym *keysym)
{

  switch (keysym->sym) {
    case SDLK_UP:
      direction &= ~PHYSICS_DIRECTION_FORWARD;
      break;
    case SDLK_DOWN:
      direction &= ~PHYSICS_DIRECTION_BACK;
      break;
    case SDLK_RIGHT:
      direction &= ~PHYSICS_DIRECTION_RIGHT;
      break;
    case SDLK_LEFT:
      direction &= ~PHYSICS_DIRECTION_LEFT;
      break;
    case SDLK_1:
      direction &= ~PHYSICS_DIRECTION_STRAFE_LEFT;
      break;
    case SDLK_2:
      direction &= ~PHYSICS_DIRECTION_STRAFE_RIGHT;
      break;
    default:
      break;
  }

  character->move(static_cast<Physics_Direction>(direction));
}


void physics_update()
{
 // Physics &physics = fragmic.physics_get();

}



int main() 
{
  Transform t;
  Scene &scene = fragmic.scene_get();
  Window &window = fragmic.window_get();
  Physics &physics = fragmic.physics_get();

  window.keyboard_pressed_callback_set(keyboard_pressed_cb);
  window.keyboard_released_callback_set(keyboard_released_cb);
  physics.custom_step_callback_set(physics_update);

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
  Node &box_root = scene.load_model("data/game_assets/", "box.dae");
  Node *cube = scene.node_find(&box_root, "Cube");
  if (cube) {
    character = physics.character_controller_add(*cube, *cube);
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
    } else {
      std::cout << "Could not find node" << std::endl;
    }
  }


  scene.scene_graph_print();

  fragmic.run();
  fragmic.term();

  return true;
}
