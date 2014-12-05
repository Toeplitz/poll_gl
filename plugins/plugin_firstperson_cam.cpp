#include "plugin_firstperson_cam.h"



Plugin_Firstperson_Camera::Plugin_Firstperson_Camera(Scene &scene, Node *node)
{
  this->scene = &scene;

  n = node;
  camera = n->camera_get();

  common_fpcamera_defaults_set();
}


void Plugin_Firstperson_Camera::cb_keyboard_pressed(QWidget *w, QKeyEvent *e)
{
  //if (console->active())
   // return;

  switch (e->key()) {
    case Qt::Key_W:
      common_fpcamera_move_add(FORWARD);
      break;
    case Qt::Key_Q:
      common_fpcamera_move_add(SIDESTEP_LEFT);
      break;
    case Qt::Key_S:
      common_fpcamera_move_add(BACKWARD);
      break;
    case Qt::Key_E:
      common_fpcamera_move_add(SIDESTEP_RIGHT);
      break;
    case Qt::Key_M:
      if (!mouse_view_toggle) {
        w->setCursor(Qt::BlankCursor);
        w->setMouseTracking(true);
        w->grabMouse();
      } else {
        w->setCursor(Qt::ArrowCursor);
        w->setMouseTracking(false);
        w->releaseMouse();
      }
      mouse_view_toggle = !mouse_view_toggle;
      break;
    default:
      break;
  }
}


void Plugin_Firstperson_Camera::cb_keyboard_released(QWidget *w, QKeyEvent *e)
{
 // if (console->active())
 //   return;

  switch (e->key()) {
    case Qt::Key_W:
      common_fpcamera_move_delete(FORWARD);
      break;
    case Qt::Key_Q:
      common_fpcamera_move_delete(SIDESTEP_LEFT);
      break;
    case Qt::Key_S:
      common_fpcamera_move_delete(BACKWARD);
      break;
    case Qt::Key_E:
      common_fpcamera_move_delete(SIDESTEP_RIGHT);
      break;
    default:
      break;
  }
}



/*
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

*/

void Plugin_Firstperson_Camera::cb_mouse_motion(QWidget *w, QMouseEvent *ev, const double dt)
{
  if (!mouse_view_toggle)
    return;

  auto width = w->width();
  auto height = w->height();

  QCursor c = w->cursor();
  c.setPos(w->mapToGlobal(QPoint((float) width / 2.f, (float) height / 2.f)));
  w->setCursor(c);
  common_fpcamera_mouse_update(ev->x(), ev->y(), width, height, dt);
}


void Plugin_Firstperson_Camera::cb_custom(const float dt)
{
  common_fpcamera_moves_process(dt);
}


void Plugin_Firstperson_Camera::common_fpcamera_defaults_set()
{
  horizontal_angle = 3.14f;
  vertical_angle = 0; 
  speed = 10.f;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
  mouse_speed = 0.2f;
#else
  mouse_speed = 0.1f;
#endif

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


void Plugin_Firstperson_Camera::common_fpcamera_mouse_update(int x, int y, int width, int height, const double dt)
{
  POLL_DEBUG(std::cout, "x, y, width, height: " << x << " , " << y);
  horizontal_angle += dt * mouse_speed * float(width / 2 - x);
  vertical_angle += dt * mouse_speed * float(height / 2 - y);

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


