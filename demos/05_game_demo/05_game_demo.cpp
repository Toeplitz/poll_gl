#include "fragmic.h"
#include "common.h"
#include "physics.h"
#include <iostream>


Fragmic fragmic("Demo 4", 1280, 720);
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
  Scene &scene = fragmic.scene_get();
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
}


static void keyboard_released_cb(SDL_Keysym *keysym)
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
  Scene &scene = fragmic.scene_get();
  Assets &assets = scene.assets_get();
  Window &window = fragmic.window_get();
  Physics &physics = fragmic.physics_get();

  window.joystick_axis_motion_callback_set(joystick_axis_motion_cb);
  window.joystick_pressed_callback_set(joystick_button_pressed_cb);
  window.joystick_released_callback_set(joystick_button_released_cb);
  window.keyboard_pressed_callback_set(keyboard_pressed_cb);
  window.keyboard_released_callback_set(keyboard_released_cb);
  physics.custom_step_callback_set(physics_update);

  {
    Node *camera_node = scene.node_camera_get();
    common_init(fragmic);
    common_fpcamera_use(camera_node);
    common_debug_use();
  }

  /* Setup room */
  room = &scene.model_load("data/game_assets/", "Room.dae", MODEL_IMPORT_OPTIMIZED | MODEL_IMPORT_LIGHTS);
  physics.collision_shape_add(*room, PHYSICS_COLLISION_TRIANGLE_MESH, true, 0);

  /* Setup panda character */
  {
    Node &panda_root = scene.model_load("data/game_assets/characters/panda/", "PandaSingle.dae", MODEL_IMPORT_OPTIMIZED);
    Node &panda_collision_root = scene.model_load("data/game_assets/characters/panda/", 
        "Panda_convex_hull.dae", MODEL_IMPORT_OPTIMIZED | MODEL_IMPORT_NO_DRAW);
    panda = scene.node_find(&panda_root, "Panda");
    Node *panda_collision = scene.node_find(&panda_collision_root, "Panda_convex_hull");
    if (panda && panda_collision) {
      armature = panda->armature;
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
  }


  /* Setup lights */
  Node *sphere = &scene.model_load("data/", "sphere.obj", MODEL_IMPORT_OPTIMIZED | MODEL_IMPORT_NO_DRAW);

  Node *box = scene.node_create("box");
  Mesh *mesh = box->mesh_create(scene.assets_get());
  mesh->cube_generate(1);

  {
    glm::vec3 light_positions[5] = {
      glm::vec3(0, 20, 0),
      glm::vec3(-50, 20, 45),
      glm::vec3(-50, 20, -45),
      glm::vec3(50, 20, 45),
      glm::vec3(50, 20, -45)
    };

    for (int i = 0; i < 5; i++) {
      Node *node = scene.node_create("Light_Directional");
      light_directional = node->light_create(assets);
      light_directional->properties_position_set(light_positions[i]);
      node->light_volume_mesh_create_from_node(sphere);
      light_directional->scale(glm::vec3(20, 20, 20));
    }
  }

  {
    Node *node = scene.node_find(room, "Spot_Light");
    light_spot = node->light_get();
    light_spot->bias_set(glm::vec3(0, 20, 0));
    light_spot->properties_position_set(glm::vec3(0, 20, 0));
    node->light_volume_mesh_create_from_node(box);
    light_spot->node_follow_set(panda);
    light_spot->scale(glm::vec3(20, 20, 20));
  }

  {
    Node *node = scene.node_find(room, "Point_Light");
    light_point = node->light_get();
    light_point->properties_position_set(glm::vec3(30, 20, 20));
    node->light_volume_mesh_create_from_node(sphere);
    light_point->scale(glm::vec3(25, 25, 25));
  }

  scene.scene_graph_print(true);
  scene.assets_get().print_all(scene.node_root_get());

  fragmic.run();
  fragmic.term();

  return true;
}
