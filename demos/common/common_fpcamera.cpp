#include "common.h"
#include "fragmic.h"

enum Camera_Move { FORWARD, BACKWARD, SIDESTEP_RIGHT, SIDESTEP_LEFT};

static Node *n;
static Camera *camera;
static float horizontal_angle;
static float vertical_angle;
static float speed;
static float mouse_speed;
static std::vector<Camera_Move> move_queue;
static glm::vec3 up;
static glm::vec3 direction;
static glm::vec3 right;
static glm::vec3 position;
static glm::vec3 target;
static bool mouse_view_toggle;


static void common_fpcamera_defaults_set();
static void common_fpcamera_directions_calc();
static void common_fpcamera_event_cb(SDL_Event *event);
static void common_fpcamera_keyboard_pressed_cb(SDL_Keysym *keysym);
static void common_fpcamera_keyboard_released_cb(SDL_Keysym *keysym);
static void common_fpcamera_move_add(Camera_Move move);
static void common_fpcamera_move_delete(Camera_Move move);
static void common_fpcamera_mouse_update(int x, int y, int width, int height);
static void common_fpcamera_moves_process();
static void common_fpcamera_mouse_button_down(SDL_MouseButtonEvent *ev);
static void common_fpcamera_mouse_button_up(SDL_MouseButtonEvent *ev);
static void common_fpcamera_mouse_motion(SDL_MouseMotionEvent *ev);
static void common_fpcamera_update_cb();


static void common_fpcamera_defaults_set()
{
  horizontal_angle = 3.14;
  vertical_angle = 0; 
  speed = 0.5f;
  mouse_speed = 0.0005f;
  position = glm::vec3(0, 5, 16);
  target = glm::vec3(0, 0, 0);
  up = glm::vec3(0, 1, 0);
  common_fpcamera_directions_calc();
  camera->transform_view_create(position, direction);
}


static void common_fpcamera_directions_calc() 
{
  direction = glm::vec3(cosf(vertical_angle) * sinf(horizontal_angle), 
      sinf(vertical_angle),
      cosf(vertical_angle) * cosf(horizontal_angle));
  right = glm::vec3(sinf(horizontal_angle - 3.14f / 2.0f),
      0, cosf(horizontal_angle - 3.14f / 2.0f) );
  up = glm::cross(right, direction);
}  


static void common_fpcamera_event_cb(SDL_Event *event)
{
  switch (event->type) {
    case SDL_MOUSEBUTTONDOWN:
      common_fpcamera_mouse_button_down(&event->button);
      break;
    case SDL_MOUSEBUTTONUP:
      common_fpcamera_mouse_button_up(&event->button);
      break;
    case SDL_MOUSEMOTION:
      common_fpcamera_mouse_motion(&event->motion);
      break;
  }
}


static void common_fpcamera_keyboard_pressed_cb(SDL_Keysym *keysym)
{
  Window &window = f->window_get();

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
      window.mouse_cursor_toggle();
      mouse_view_toggle = !mouse_view_toggle;
      break;
    default:
      break;
  }
}


static void common_fpcamera_keyboard_released_cb(SDL_Keysym *keysym)
{
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


static void common_fpcamera_move_add(Camera_Move move) 
{
  int n = move_queue.size();
  for (int i = 0; i < n; i++) {
    if (move_queue[i] == move)
      return;
  }
  move_queue.push_back(move);
  common_fpcamera_moves_process();
}


static void common_fpcamera_move_delete(Camera_Move move) 
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

  common_fpcamera_moves_process();
}


static void common_fpcamera_mouse_update(int x, int y, int width, int height) 
{
  horizontal_angle += mouse_speed * float(width / 2 - x);
  vertical_angle += mouse_speed * float(height / 2 - y);

  common_fpcamera_directions_calc();
  camera->transform_view_create(position, direction);
}


static void common_fpcamera_moves_process() 
{
  int n = move_queue.size();
  if (n == 0) return;

  common_fpcamera_directions_calc();

  for (int i = 0; i < n; i++) {
    switch (move_queue[i]) {
      case FORWARD:
        position += direction * speed;
        break;
      case BACKWARD:
        position -= direction * speed;
        break;
      case SIDESTEP_RIGHT:
        position += right * speed;
        break;
      case SIDESTEP_LEFT:
        position -= right * speed;
        break;
      default:
        fprintf(stderr, "Fragmic error: move is not implemented\n");
        break;
    }
  }

  camera->transform_view_create(position, direction);
}


static void common_fpcamera_mouse_button_down(SDL_MouseButtonEvent *ev)
{
  if (ev->button != 3 || !mouse_view_toggle)
    return;
  common_fpcamera_move_add(FORWARD);
}


static void common_fpcamera_mouse_button_up(SDL_MouseButtonEvent *ev)
{
  if (ev->button != 3 || !mouse_view_toggle)
    return;
  common_fpcamera_move_delete(FORWARD);
}


static void common_fpcamera_mouse_motion(SDL_MouseMotionEvent *ev)
{
  static int last_x, last_y;
  Window &window = f->window_get();

  if (!mouse_view_toggle)
    return;

  if (!last_x)
    last_x = ev->x;
  if (!last_y)
    last_y = ev->y;

  common_fpcamera_mouse_update(ev->x, ev->y, window.width, window.height);
  window.mouse_cursor_center();

  last_x = ev->x;
  last_y = ev->y;
}


static void common_fpcamera_update_cb()
{
  common_fpcamera_moves_process();
}


void common_fpcamera_use(Node *node)
{
  Window &window = f->window_get();
  n = node;
  camera = n->camera_get();

  if (n->camera_get()) {
    std::cout << "Error: no camera attached to this node" << std::endl;
  }

  common_fpcamera_defaults_set();
  camera->update_callback_set(common_fpcamera_update_cb);
  window.keyboard_pressed_callback_set(common_fpcamera_keyboard_pressed_cb);
  window.keyboard_released_callback_set(common_fpcamera_keyboard_released_cb);
  window.event_callback_set(common_fpcamera_event_cb);
}
