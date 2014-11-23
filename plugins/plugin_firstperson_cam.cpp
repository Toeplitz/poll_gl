#include "plugin_firstperson_cam.h"



Plugin_Firstperson_Camera::Plugin_Firstperson_Camera(Console &console, Scene &scene, Node *node)
{
  this->console = &console;
  this->scene = &scene;
 // this->window = &scene.window_get();

  n = node;
  camera = n->camera_get();

  common_fpcamera_defaults_set();
}


/*
void Plugin_Firstperson_Camera::cb_keyboard_pressed(SDL_Keysym *keysym)
{
  if (console->active()) 
    return;

  switch (keysym->sym) {
    case SDLK_w:
      common_fpcamera_move_add(FORWARD);
      break;
    case SDLK_q:
      common_fpcamera_move_add(SIDESTEP_LEFT);
      break;
    case SDLK_s:
      common_fpcamera_move_add(BACKWARD);
      break;
    case SDLK_e:
      common_fpcamera_move_add(SIDESTEP_RIGHT);
      break;
    case SDLK_m:
      window->mouse_cursor_toggle();
      mouse_view_toggle = !mouse_view_toggle;
      break;
    default:
      break;
  }
}


void Plugin_Firstperson_Camera::cb_keyboard_released(SDL_Keysym *keysym)
{
  if (console->active()) 
    return;

  switch (keysym->sym) {
    case SDLK_w:
      common_fpcamera_move_delete(FORWARD);
      break;
    case SDLK_q:
      common_fpcamera_move_delete(SIDESTEP_LEFT);
      break;
    case SDLK_s:
      common_fpcamera_move_delete(BACKWARD);
      break;
    case SDLK_e:
      common_fpcamera_move_delete(SIDESTEP_RIGHT);
      break;
    default:
      break;
  }
}



void Plugin_Firstperson_Camera::cb_mouse_pressed(SDL_MouseButtonEvent *ev)
{
  if (ev->button != 3 || !mouse_view_toggle)
    return;
  common_fpcamera_move_add(FORWARD);
}


void Plugin_Firstperson_Camera::cb_mouse_released(SDL_MouseButtonEvent *ev)
{
  if (ev->button != 3 || !mouse_view_toggle)
    return;
  common_fpcamera_move_delete(FORWARD);
}


void Plugin_Firstperson_Camera::cb_mouse_motion(SDL_MouseMotionEvent *ev)
{
  static int last_x, last_y;

  if (!mouse_view_toggle)
    return;

  if (!last_x)
    last_x = ev->x;
  if (!last_y)
    last_y = ev->y;

  common_fpcamera_mouse_update(ev->x, ev->y, window->width_get(), window->height_get());
  window->mouse_cursor_center();

  last_x = ev->x;
  last_y = ev->y;
}

*/


void Plugin_Firstperson_Camera::cb_custom(const float dt)
{
  common_fpcamera_moves_process(dt);
}




void Plugin_Firstperson_Camera::common_fpcamera_defaults_set()
{
  horizontal_angle = 3.14;
  vertical_angle = 0; 
  speed = 10.f;
  mouse_speed = 0.00025f;
  position = glm::vec3(0, 5, 16);
  target = glm::vec3(0, 0, 0);
  up = glm::vec3(0, 1, 0);
  common_fpcamera_directions_calc();
  camera->transform_view_create(position, direction);
}


void Plugin_Firstperson_Camera::common_fpcamera_directions_calc() 
{
  direction = glm::vec3(cosf(vertical_angle) * sinf(horizontal_angle), 
      sinf(vertical_angle),
      cosf(vertical_angle) * cosf(horizontal_angle));
  right = glm::vec3(sinf(horizontal_angle - 3.14f / 2.0f),
      0, cosf(horizontal_angle - 3.14f / 2.0f) );
  up = glm::cross(right, direction);
}  



void Plugin_Firstperson_Camera::common_fpcamera_move_add(Camera_Move move) 
{
  int n = move_queue.size();
  for (int i = 0; i < n; i++) {
    if (move_queue[i] == move)
      return;
  }
  move_queue.push_back(move);
 // common_fpcamera_moves_process();
}


void Plugin_Firstperson_Camera::common_fpcamera_move_delete(Camera_Move move) 
{
  int unique_hit = -1;
  int n = move_queue.size();
  for (int i = 0; i < n; i++) {
    if (move_queue[i] == move) {
      unique_hit = i;
      break;
    }
  }
  if (unique_hit >= 0)
    move_queue.erase(move_queue.begin() + unique_hit);

 // common_fpcamera_moves_process();
}


void Plugin_Firstperson_Camera::common_fpcamera_mouse_update(int x, int y, int width, int height) 
{
  horizontal_angle += mouse_speed * float(width / 2 - x);
  vertical_angle += mouse_speed * float(height / 2 - y);

  common_fpcamera_directions_calc();
  camera->transform_view_create(position, direction);
}


void Plugin_Firstperson_Camera::common_fpcamera_moves_process(const float dt) 
{
  int n = move_queue.size();
  if (n == 0) return;

  common_fpcamera_directions_calc();
  float dt_speed = speed * dt;

  for (int i = 0; i < n; i++) {
    switch (move_queue[i]) {
      case FORWARD:
        position += direction * dt_speed;
        break;
      case BACKWARD:
        position -= direction * dt_speed;
        break;
      case SIDESTEP_RIGHT:
        position += right * dt_speed;
        break;
      case SIDESTEP_LEFT:
        position -= right * dt_speed;
        break;
      default:
        fprintf(stderr, "Fragmic error: move is not implemented\n");
        break;
    }
  }

  camera->transform_view_create(position, direction);
}


