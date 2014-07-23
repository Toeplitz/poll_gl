#include "fpcamera.h"
#include "fragmic.h"

enum Camera_Move { FORWARD, BACKWARD, SIDESTEP_RIGHT, SIDESTEP_LEFT};


Fragmic *f;
Node *n;
Camera *camera;

float horizontal_angle;
float vertical_angle;
float speed;
float mouse_speed;

static std::vector<Camera_Move> move_queue;

glm::vec3 up;
glm::vec3 direction;
glm::vec3 right;
glm::vec3 position;
glm::vec3 target;
bool mouse_view_toggle;


static void fpcamera_defaults_set();
static void fpcamera_move_add(Camera_Move move);
static void fpcamera_move_delete(Camera_Move move);
static void fpcamera_mouse_update(int x, int y, int width, int height);
static void fpcamera_moves_process();
static void fpcamera_directions_calc();
static void fpcamera_mouse_button_down(SDL_MouseButtonEvent *ev);
static void fpcamera_mouse_button_up(SDL_MouseButtonEvent *ev);
static void fpcamera_mouse_motion(SDL_MouseMotionEvent *ev);


static void fpcamera_defaults_set()
{
  horizontal_angle = 3.14;
  vertical_angle = 0; 
  speed = 0.5f;
  mouse_speed = 0.0005f;
  position = glm::vec3(0, 5, 16);
  target = glm::vec3(0, 0, 0);
  up = glm::vec3(0, 1, 0);
  fpcamera_directions_calc();
  camera->transform_view_update(position, direction);
}


static void fpcamera_keyboard_pressed_cb(SDL_Keysym *keysym)
{
  Physics &physics = f->physics_get();
  Window &window = f->window_get();

  switch (keysym->sym) {
    case SDLK_SPACE:
      physics.pause();
      break;
    case SDLK_o:
      physics.debug();
      break;
    case SDLK_w:
      fpcamera_move_add(FORWARD);
      break;
    case SDLK_q:
      fpcamera_move_add(SIDESTEP_LEFT);
      break;
    case SDLK_s:
      fpcamera_move_add(BACKWARD);
      break;
    case SDLK_e:
      fpcamera_move_add(SIDESTEP_RIGHT);
      break;
    case SDLK_m:
      window.mouse_cursor_toggle();
      mouse_view_toggle = !mouse_view_toggle;
      break;
    default:
      break;
  }
}


static void fpcamera_keyboard_released_cb(SDL_Keysym *keysym)
{
  switch (keysym->sym) {
    case SDLK_w:
      fpcamera_move_delete(FORWARD);
      break;
    case SDLK_q:
      fpcamera_move_delete(SIDESTEP_LEFT);
      break;
    case SDLK_s:
      fpcamera_move_delete(BACKWARD);
      break;
    case SDLK_e:
      fpcamera_move_delete(SIDESTEP_RIGHT);
      break;
    default:
      break;
  }
}


static void fpcamera_event_cb(SDL_Event *event)
{
  switch (event->type) {
    case SDL_MOUSEBUTTONDOWN:
      fpcamera_mouse_button_down(&event->button);
      break;
    case SDL_MOUSEBUTTONUP:
      fpcamera_mouse_button_up(&event->button);
      break;
    case SDL_MOUSEMOTION:
      fpcamera_mouse_motion(&event->motion);
      break;
  }

}


static void fpcamera_move_add(Camera_Move move) 
{
  int n = move_queue.size();
  for (int i = 0; i < n; i++) {
    if (move_queue[i] == move)
      return;
  }
  move_queue.push_back(move);
  fpcamera_moves_process();
}


static void fpcamera_move_delete(Camera_Move move) 
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

  fpcamera_moves_process();
}


static void fpcamera_mouse_update(int x, int y, int width, int height) 
{
  horizontal_angle += mouse_speed * float(width / 2 - x);
  vertical_angle += mouse_speed * float(height / 2 - y);

  fpcamera_directions_calc();
  camera->transform_view_update(position, direction);
}


static void fpcamera_moves_process() 
{
  int n = move_queue.size();
  if (n == 0) return;

  fpcamera_directions_calc();

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

  camera->transform_view_update(position, direction);
}


static void fpcamera_mouse_button_down(SDL_MouseButtonEvent *ev)
{
  if (ev->button != 3 || !mouse_view_toggle)
    return;
  fpcamera_move_add(FORWARD);
}


static void fpcamera_mouse_button_up(SDL_MouseButtonEvent *ev)
{
  if (ev->button != 3 || !mouse_view_toggle)
    return;
  fpcamera_move_delete(FORWARD);
}


static void fpcamera_directions_calc() 
{
  direction = glm::vec3(cosf(vertical_angle) * sinf(horizontal_angle), 
      sinf(vertical_angle),
      cosf(vertical_angle) * cosf(horizontal_angle));
  right = glm::vec3(sinf(horizontal_angle - 3.14f / 2.0f),
      0, cosf(horizontal_angle - 3.14f / 2.0f) );
  up = glm::cross(right, direction);
}  


static void fpcamera_mouse_motion(SDL_MouseMotionEvent *ev)
{
  static int last_x, last_y;
  Window &window = f->window_get();

  if (!mouse_view_toggle)
    return;

  if (!last_x)
    last_x = ev->x;
  if (!last_y)
    last_y = ev->y;

  fpcamera_mouse_update(ev->x, ev->y, window.width, window.height);
  window.mouse_cursor_center();

  last_x = ev->x;
  last_y = ev->y;
}


void fpcamera_update_cb()
{
  fpcamera_moves_process();
}


void fpcamera_use(Fragmic &fragmic, Node *node)
{
  Window &window = fragmic.window_get();
  f = &fragmic;
  n = node;
  camera = n->camera_get();

  if (n->camera_get()) {
    std::cout << "Error: no camera attached to this node" << std::endl;
  }

  fpcamera_defaults_set();
  camera->update_callback_set(fpcamera_update_cb);
  window.keyboard_pressed_callback_set(fpcamera_keyboard_pressed_cb);
  window.keyboard_released_callback_set(fpcamera_keyboard_released_cb);
  window.event_callback_set(fpcamera_event_cb);

}
