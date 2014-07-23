#include "camera.h"
#include "utils.h"


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Camera::Camera(const int width, const int height) 
{
  defaults_set();
  transform_perspective_create(width, height);
}   


Camera::~Camera(void) 
{
  std::cout << "Delete camera" << std::endl;
}  


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Camera::move_add(Camera_Move move) 
{
  int n = this->move_queue.size();
  for (int i = 0; i < n; i++) {
    if (this->move_queue[i] == move)
      return;
  }
  this->move_queue.push_back(move);
  moves_process();
}

void Camera::move_delete(Camera_Move move) 
{
  int unique_hit = -1;
  int n = this->move_queue.size();
  for (int i = 0; i < n; i++) {
    if (this->move_queue[i] == move) {
      unique_hit = i;
      break;
    }
  }
  if (unique_hit >= 0)
    this->move_queue.erase(this->move_queue.begin() + unique_hit);
  moves_process();
}


void Camera::update(const double dt) 
{
  moves_process();
} 

  
bool Camera::mouse_update(int xPos, int yPos, int width, int height) 
{
  horizontalAngle += mouseSpeed * float (width / 2 - xPos);
  verticalAngle += mouseSpeed * float (height / 2 - yPos);

  directions_calc();
  transform_view_update();

  return true;
}


/**************************************************/
/***************** PRIVATE METHODS ****************/
/**************************************************/


void Camera::defaults_set(void) 
{
  horizontalAngle = 3.14;
  verticalAngle = 0; 
  speed = 0.5f;
  fov = 45.f;
 // speed = 10.5f;
  mouseSpeed = 0.0005f;
  position = glm::vec3(0, 5, 16);
  target = glm::vec3(0, 0, 0);
  up = glm::vec3(0, 1, 0);
  directions_calc();
  transform_view_update();
}  


void Camera::directions_calc() 
{
  direction = glm::vec3(cosf(verticalAngle) * sinf(horizontalAngle), sinf(verticalAngle),
        cosf(verticalAngle) * cosf(horizontalAngle));
  right = glm::vec3(sinf(horizontalAngle - 3.14f / 2.0f), 0, cosf(horizontalAngle - 3.14f / 2.0f) );
  up = glm::cross(right, direction);
 // std::cout << position.x << ", " << position.y << ", " << position.z << std::endl;
}  


bool Camera::moves_process(void) 
{
  int n = move_queue.size();
  if (n == 0)
    return false;
  directions_calc();
  for (int i = 0; i < n; i++) {
    switch (this->move_queue[i]) {
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
  transform_view_update();
  return true;
}


void Camera::transform_perspective_create(float width, float height) 
{
  perspective = glm::perspective(fov, width / height, 1.0f, 2500.0f);
}


void Camera::transform_view_update() 
{
  view = glm::lookAt(position, position + direction, up);
}  
