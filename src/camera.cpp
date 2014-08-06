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


void Camera::fov_set(const float fov)
{
  this->fov = fov;
  transform_perspective_create(width, height);
}


void Camera::transform_perspective_create(const int width, const int height)
{
  this->width = width;
  this->height = height;

  perspective = glm::perspective(fov, (float) width / (float) height, 1.0f, 500.0f);
  inv_perspective = glm::inverse(inv_perspective);
}


glm::mat4 &Camera::transform_perspective_get()
{
  return perspective;
}


glm::mat4 &Camera::transform_perspective_inverse_get()
{
  return inv_perspective;
}


void Camera::transform_view_create(const glm::vec3 position, const glm::vec3 direction) 
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


