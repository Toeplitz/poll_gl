#include "camera.h"


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Camera::fov_set(const float fov)
{
  this->fov = fov;
  transform_perspective_create(width, height);
}


const glm::vec3 &Camera::position_get()
{
  return position;
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
  this->position = position;
  transform_view_projection_update();
}


glm::mat4 &Camera::transform_view_get()
{
  return view;
}


glm::mat4 &Camera::transform_view_projection_get()
{
  return view_projection;
}


glm::mat4 &Camera::transform_view_projection_update()
{
  view_projection = perspective * view;
  return view_projection;
}


