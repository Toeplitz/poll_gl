#include "poll.h"
#include "poll_plugin.h"
#include "plugin_debug.h"
#include "plugin_firstperson_cam.h"
#include "physics.h"
#include <iostream>


Poll poll;
Physics_Character_Controller *character;
unsigned int direction = 0;
Armature *armature = nullptr;

Node *panda;
Node *room;


Light *light_directional;
Light *light_point;
Light *light_spot;


static void light_toggle(Light *light)
{
  Scene &scene = poll.scene_get();
  Assets &assets = scene.assets_get();

  int r = assets.light_is_active(light);
  if (r) {
    assets.light_deactivate(light);
  } else {
    assets.light_activate(light);
  }
}



static void joystick_axis_motion_cb(SDL_JoyAxisEvent *ev)
{
  static int joystick_x = 0;
  static int joystick_y = 0;
  Window &window = poll.window_get();
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


static void joystick_button_pressed_cb(SDL_JoyButtonEvent *ev)
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


static void joystick_button_released_cb(SDL_JoyButtonEvent *ev)
{
}


static void keyboard_pressed_cb(SDL_Keysym *keysym)
{
  /*

  if (console.active()) 
    return;

  switch (keysym->sym) {
    case SDLK_SPACE:
      character->jump();
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
    case SDLK_9:
      light_toggle(light_directional);
      break;
    case SDLK_8:
      light_toggle(light_point);
      break;
    case SDLK_7:
      light_toggle(light_spot);
      break;
    default:
      break;
  }

  character->move(static_cast<Physics_Direction>(direction));
  */
}


static void keyboard_released_cb(SDL_Keysym *keysym)
{
  /*
  Console &console = p->console_get();

  if (console.active()) 
    return;


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

  */
}


static void physics_update()
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
  Scene &scene = poll.scene_get();
  Assets &assets = scene.assets_get();
  Window &window = poll.window_get();
  Physics &physics = poll.physics_get();
  GLcontext glcontext = poll.glcontext_get();

  /*
  window.joystick_axis_motion_callback_set(joystick_axis_motion_cb);
  window.joystick_pressed_callback_set(joystick_button_pressed_cb);
  window.joystick_released_callback_set(joystick_button_released_cb);
  window.keyboard_pressed_callback_set(keyboard_pressed_cb);
  window.keyboard_released_callback_set(keyboard_released_cb);
  physics.custom_step_callback_set(physics_update);
*/

  Node *camera_node = scene.node_camera_get();
  auto firstperson_camera = std::unique_ptr<Plugin_Firstperson_Camera>(new Plugin_Firstperson_Camera(poll.console_get(), scene, camera_node));
  poll.plugin_add(*firstperson_camera);

  auto debug = std::unique_ptr<Plugin_Debug>(new Plugin_Debug(poll.console_get(), scene));
  poll.plugin_add(*debug);

  {
    room = &scene.load("data/game_assets/", "Room.dae", MODEL_IMPORT_OPTIMIZED);
    room->physics_rigidbody_create(scene, true, Physics_Rigidbody::TRIANGLE_MESH, Physics_Rigidbody::DYNAMIC, 0);
  }

  Node *foo = &scene.load("data/", "cone.dae", MODEL_IMPORT_OPTIMIZED);
  foo->translate(scene, glm::vec3(0, 0, 40));
  foo->physics_rigidbody_create(scene, false, Physics_Rigidbody::CONVEX_HULL, Physics_Rigidbody::DYNAMIC, 1.f);

  {
    Node &node = scene.load("data/", "sphere.obj", MODEL_IMPORT_OPTIMIZED);
    node.translate(scene, glm::vec3(2, 27, 40));
    node.physics_rigidbody_create(scene, false, Physics_Rigidbody::BOX, Physics_Rigidbody::DYNAMIC, 1.f);
  }


  /* Setup panda character */
  {
//    Node &panda_root = scene.load("data/game_assets/characters/panda/", "PandaSingle.dae", MODEL_IMPORT_OPTIMIZED);
 //   panda_root.physics_rigidbody_create(scene, true, Physics_Rigidbody::CONVEX_HULL, Physics_Rigidbody::DYNAMIC, 0.f);

    /*
      Physics_Rigidbody *rigidbody = scene.node_find(&panda_root, "Panda")->physics_rigidbody_get();
      if (rigidbody) {
        rigidbody->mass_set(&physics, 1);
      }
*/
    /*
    Node &panda_collision_root = scene.load(glcontext, "data/game_assets/characters/panda/", 
        "Panda_convex_hull.dae", MODEL_IMPORT_OPTIMIZED | MODEL_IMPORT_NO_DRAW);
    panda = scene.node_find(&panda_root, "Panda");
    Node *panda_collision = scene.node_find(&panda_collision_root, "Panda_convex_hull");
    if (panda && panda_collision) {
      armature = panda->armature_get();
      character = physics.character_controller_add(*panda, *panda_collision);
      armature->keyframe_range_set("bind", 0, 0);
      armature->keyframe_range_set("idle", 1, 3);
      armature->keyframe_range_set("walk", 4, 10);
      armature->keyframe_range_set("run", 11, 18);
      armature->keyframe_range_set("jump", 22, 22); // Only include 1 frame for now.
      armature->keyframe_range_set("collapse", 24, 33); 
    } else {
      std::cout << "Could not find node" << std::endl;
    }
    */
  }


  /* Setup lights */
  {

    glm::vec3 light_positions[5] = {
      glm::vec3(0, 21, 0),
      glm::vec3(-50, 15, 45),
      glm::vec3(-50, 15, -45),
      glm::vec3(50, 15, 45),
      glm::vec3(50, 15, -45)
    };

    glm::vec3 light_color[5] = {
      glm::vec3(1, 1, 1),
      glm::vec3(1, 0, 0),
      glm::vec3(0, 1, 0),
      glm::vec3(0, 0, 1),
      glm::vec3(1, 0.5, 0)
    };

    for (int i = 0; i < 5; i++) {
      Node *node = scene.node_create("Light_Point");
      Light *light = node->light_create(scene, Light::POINT);
      light->properties_color_set(light_color[i]);
      node->translate(scene, light_positions[i]);
      node->scale(scene, glm::vec3(30, 30, 30));
    }

    {
      Node *node = scene.node_create("Light_Directionl_Global");
      Light *light = node->light_create(scene, Light::DIRECTIONAL, Light::GLOBAL);
      node->translate(scene, glm::vec3(0, 40, 0));
      light->properties_direction_set(glm::vec3(0, -1, -1));
      light->properties_color_set(glm::vec3(0.5, 0.5, 0.5));
    }

    {
      Node *node = scene.node_create("Light_Spot");
      Light *light = node->light_create(scene, Light::SPOT);
      node->translate(scene, glm::vec3(-40, 20, 0));
      node->scale(scene, glm::vec3(30, 30, 30));
      light->properties_color_set(glm::vec3(1, 0, 0));
    }
  }


  poll.run();
  poll.term();

  return true;
}
