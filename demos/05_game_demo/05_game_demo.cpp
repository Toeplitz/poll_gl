#include "fragmic.h"
#include "physics.h"
#include <iostream>


Fragmic fragmic("Demo 4", 1280, 720);
Physics_Character_Controller *character;
unsigned int direction = 0;
Armature *armature = nullptr;


void joystick_axis_motion_cb(SDL_JoyAxisEvent *ev)
{
  static int joystick_x = 0;
  static int joystick_y = 0;
  Window &window = fragmic.window_get();
  float r, angle;

  direction |= PHYSICS_DIRECTION_ROTATE;
  direction |= PHYSICS_DIRECTION_FORWARD;

  switch (ev->axis) {
    case 0: // x-axisa
      joystick_x = ev->value;
      break;
    case 1: // y-axis
      joystick_y = ev->value;
      joystick_y = joystick_y * (-1.f);
      break;
    default:
      break;
  }

  angle = window.joystick_angle_get(joystick_x, joystick_y, &r);
  if (isnan(angle))
    return;

   if (r > 0.1) {
    character->joystick_angle_set(angle);
    character->move(static_cast<Physics_Direction>(direction));
   } else {
    direction &= ~PHYSICS_DIRECTION_ROTATE;
    direction &= ~PHYSICS_DIRECTION_FORWARD;
    character->joystick_angle_set(0.f);
    character->move(static_cast<Physics_Direction>(direction));
   }

}


void joystick_button_pressed_cb(SDL_JoyButtonEvent *ev)
{
  std::cout << "Button: " << ev->button << std::endl;

  switch (ev->button) {
    case 1: // B button
      character->jump();
      break;
    default:
      break;
  }

}


void joystick_button_released_cb(SDL_JoyButtonEvent *ev)
{
}


void keyboard_pressed_cb(SDL_Keysym *keysym)
{
  Physics &physics = fragmic.physics_get();

  switch (keysym->sym) {
    case SDLK_SPACE:
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
  static Physics_Character_State last_state;
  Physics_Character_State cur_state = character->state_get();

  if (last_state == cur_state) return;

  switch (cur_state) {
    case CHARACTER_STATE_IDLE:
      armature->keyframe_range_activate("idle");
      break;
    case CHARACTER_STATE_MOVING:
      armature->keyframe_range_activate("run");
      break;
    case CHARACTER_STATE_JUMPING:
      armature->keyframe_range_activate("jump");
      break;
    default:
      break;
  }
  last_state = cur_state;
}



int main() 
{
  Transform t;
  Scene &scene = fragmic.scene_get();
  Window &window = fragmic.window_get();
  Physics &physics = fragmic.physics_get();

  window.joystick_axis_motion_callback_set(joystick_axis_motion_cb);
  window.joystick_pressed_callback_set(joystick_button_pressed_cb);
  window.joystick_released_callback_set(joystick_button_released_cb);
  window.keyboard_pressed_callback_set(keyboard_pressed_cb);
  window.keyboard_released_callback_set(keyboard_released_cb);
  physics.custom_step_callback_set(physics_update);

  {
    Node &node = *scene.node_create("skybox");
    node.mesh->cube_generate(40.0f);
    Material &material = *node.material;
    material.cubemap_create("data/game_assets/skybox/SkyboxSet1/DarkStormy/", "DarkStormyFront2048.png",
        "DarkStormyBack2048.png", "DarkStormyUp2048.png", "DarkStormyDown2048.png", "DarkStormyLeft2048.png", "DarkStormyRight2048.png");
    scene.upload_queue_add(node);
  }

  Node &room = scene.model_load("data/game_assets/", "Room.dae");
  physics.collision_node_add(room, PHYSICS_COLLISION_TRIANGLE_MESH, true, 0);

  /*
  Node &box_root = scene.model_load("data/game_assets/", "box.dae");
  Node *cube = scene.node_find(&box_root, "Cube");
  if (cube) {
    character = physics.character_controller_add(*cube, *cube);
  } else {
    std::cout << "Could not find node" << std::endl;
  }
  */

  {
    Node &panda_root = scene.model_load("data/game_assets/characters/panda/", "PandaSingle.dae");
    Node &panda_collision_root = scene.model_load("data/game_assets/characters/panda/", "Panda_convex_hull.dae", false);
    Node *panda = scene.node_find(&panda_root, "Panda");
    Node *panda_collision = scene.node_find(&panda_collision_root, "Panda_convex_hull");
    if (panda && panda_collision) {
      armature = panda->armature;
      character = physics.character_controller_add(*panda, *panda_collision);
      armature->keyframe_range_set("bind", 0, 0);
      armature->keyframe_range_set("idle", 1, 3);
      armature->keyframe_range_set("walk", 4, 10);
      armature->keyframe_range_set("run", 11, 18);
      armature->keyframe_range_set("jump", 22, 22); // Only include 1 frame for now.
    } else {
      std::cout << "Could not find node" << std::endl;
    }
  }

  scene.scene_graph_print();

  fragmic.run();
  fragmic.term();

  return true;
}
