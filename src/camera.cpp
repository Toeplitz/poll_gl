#include "camera.h"
#include "utils.h"


using glm::vec3;
using glm::mat4;
using glm::mat3;
using glm::cross;


Camera::Camera(int width, int height) 
{
  createOrthoProjection(width, height);
  createPerspectiveProjection(width, height);
  setStartPosition();
  //cameraPan.toggle = false;
}   

Camera::~Camera(void) 
{
  std::cout << "Delete camera" << std::endl;
}  

void Camera::addMove(CameraMove move) 
{
  int n = this->cameraMoveQueue.size();
  for (int i = 0; i < n; i++) {
    if (this->cameraMoveQueue[i] == move)
      return;
  }
  this->cameraMoveQueue.push_back(move);
  processInputMoves();
}

void Camera::deleteMove(CameraMove move) 
{
  int unique_hit = -1;
  int n = this->cameraMoveQueue.size();
  for (int i = 0; i < n; i++) {
    if (this->cameraMoveQueue[i] == move) {
      unique_hit = i;
      break;
    }
  }
  if (unique_hit >= 0)
    this->cameraMoveQueue.erase(this->cameraMoveQueue.begin() + unique_hit);
  processInputMoves();
}

bool Camera::processInputMoves(void) 
{
  int n = this->cameraMoveQueue.size();
  if (n == 0)
    return false;
  computeDirections();
  for (int i = 0; i < n; i++) {
    switch (this->cameraMoveQueue[i]) {
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
  updateView();
  return true;
}

void Camera::update(double dt) 
{
  processInputMoves();
  processWayPoints(dt);
} 

void Camera::processWayPoints(double dt) 
{
  if (dt <= 0)
    return;

  /*
  if (!cameraPan.toggle)
    return;
  computeDirections();

  if (!cameraPan.stepTime(dt)) {
    cameraPan.toggle = false;
    cameraPan.keyframe_delete_all();
    return;
  
  }
  */

 // view = glm::lookAt(cameraPan.position, target, up);
 // updateViewProjection();
}

void Camera::translate(glm::vec3 dir) 
{
  position = position + dir;
  updateView();
}   


void Camera::setWayPoints(std::vector < glm::vec3 > points,
    std::vector <Uint32> time,
    glm::vec3 _target, bool useCurPos) 
{
  /*
  assert(points.size() == time.size());
  cameraPan.toggle = true;
  target = _target;
  glm::vec3 s(0, 0, 0);
  glm::quat q(0, 0, 0, 0);
  if (useCurPos) {
    std::vector < glm::vec3 >::iterator it = points.begin();
    points.insert(it, position);
    points.push_back(position);
    std::vector < Uint32 >::iterator itt = time.begin();
    time.insert(itt, 0);
    time.push_back(time[time.size() - 1] + (time[1] - time[0]));
  }

  for (size_t i = 0; i < points.size(); i++) {
    cameraPan.keyframe_add(s, q, points[i], (double) time[i] / 1000.0);
  } 
  */
}  
  
void Camera::updateViewProjection(void) 
{
  viewProjection = perspective * view;
  //viewProjection = ortho * view;
}  

void Camera::createOrthoProjection(float width, float height) 
{
  ortho = glm::ortho(0.0f, width, height, 0.0f);
}  


void Camera::createPerspectiveProjection(float width, float height) 
{
  fov = 45.f;
  perspective = glm::perspective(fov, width / height, 1.0f, 500.0f);
}


void Camera::setStartPosition(void) 
{
  horizontalAngle = 3.14;
  verticalAngle = 0; 
  speed = 0.5f;
  mouseSpeed = 0.0005f;
  position = vec3(0, 5, 16);
  target = vec3(0, 0, 0);
  up = vec3(0, 1, 0);
  computeDirections();
  updateView();
}  


void Camera::computeDirections() 
{
  direction = vec3(cosf(verticalAngle) * sinf(horizontalAngle), sinf(verticalAngle),
        cosf(verticalAngle) * cosf(horizontalAngle));
  right = vec3(sinf(horizontalAngle - 3.14f / 2.0f), 0, cosf(horizontalAngle - 3.14f / 2.0f) );
  up = cross(right, direction);
 // std::cout << position.x << ", " << position.y << ", " << position.z << std::endl;
}  


void Camera::updateView() 
{
  view = glm::lookAt(position, position + direction, up);
  std::cout << " ------ " << std::endl;
  print_matrix(std::cout, view, 0);
  updateViewProjection();
}  


bool Camera::mouseViewUpdate(int xPos, int yPos, int width, int height) 
{
  horizontalAngle += mouseSpeed * float (width / 2 - xPos);
  verticalAngle += mouseSpeed * float (height / 2 - yPos);

  
  computeDirections();
  updateView();

  return true;
}
