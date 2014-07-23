#include "camera.h"


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Camera::Camera() 
{
  fov = 45.f;
  up = glm::vec3(0, 1, 0);
}   


Camera::~Camera() 
{
}  


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/

void Camera::transform_perspective_create(const int width, const int height)
{
  perspective = glm::perspective(fov, (float) width / height, 1.0f, 500.0f);
}


glm::mat4 &Camera::transform_perspective_get()
{
  return perspective;
}


void Camera::transform_view_update(const glm::vec3 position, const glm::vec3 direction) 
{
  view = glm::lookAt(position, position + direction, up);
}


glm::mat4 &Camera::transform_view_get()
{
  return view;
}


void Camera::update()
{
  if (update_callback)
    update_callback();
}


/**************************************************/
/***************** PRIVATE METHODS ****************/
/**************************************************/


