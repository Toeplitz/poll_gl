#include "animated.h"

#include <memory>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "utils.h"

using glm::vec3;
using glm::quat;
using glm::mix;
using glm::normalize;
using glm::mat4;


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Animated::Animated():
  animation_time_max(0),
  animation_time_min(0),
  keyframe_next(1),
  keyframe_prev(0),
  animation_time(0)
{
}


Animated::~Animated()
{
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Animated::keyframe_add(vec3 s, quat q, vec3 t, double time)
{
  std::unique_ptr <Keyframe> kf_ptr(new Keyframe(s, q, t, time));
  Keyframe &kf = *kf_ptr;
  keyframes.push_back(std::move(kf_ptr));

  if (animation_time_max < kf.time) {
    animation_time_max = kf.time;
  }
  if (animation_time_min > kf.time) {
    animation_time_min = kf.time;
  }
}


void Animated::keyframe_delete_all()
{
  keyframes.clear();
}


glm::mat4 Animated::keyframe_interpolate(double factor)
{
  vec3 tInterp, sInterp;
  quat qInterp;

  int frame_prev = keyframe_prev_get();
  int frame_next = keyframe_next_get();

  /* Scaling */
  if (keyframes[frame_prev]->s != keyframes[frame_next]->s) {
    sInterp = mix(keyframes[frame_prev]->s,
        keyframes[frame_next]->s, (float) factor);
  } else {
    sInterp = keyframes[frame_prev]->s;
  }

  /* Translation */
  if (keyframes[frame_prev]->t != keyframes[frame_next]->t) {
    tInterp = mix(keyframes[frame_prev]->t,
        keyframes[frame_next]->t, (float) factor);
  } else {
    tInterp = keyframes[frame_prev]->t;
  }

  /* Rotation */
  if (keyframes[frame_prev]->q != keyframes[frame_next]->q) {
    qInterp = mixQuat(keyframes[frame_prev]->q,
        keyframes[frame_next]->q, (float) factor);
  } else {
    qInterp = keyframes[frame_prev]->q;
  }

  mat4 m = glm::translate(glm::mat4(1), tInterp) *
    glm::mat4_cast(qInterp) * glm::scale(glm::mat4(1), sInterp);
  transform_local_interpolated = m;

  return m;
}


double Animated::keyframe_prev_time_get(void)
{
  if (keyframe_next != 0)
    return keyframes[keyframe_prev]->time;

  return this->animation_time_min;
}


double Animated::keyframe_next_time_get(void)
{
  assert(keyframes.size() > 0);

  if (keyframes[keyframe_next])
    return keyframes[keyframe_next]->time;

  return this->animation_time_max;
}


int Animated::keyframe_next_get(void)
{
  return keyframe_next;
}


int Animated::keyframe_prev_get(void)
{
  return keyframe_prev;
}


unsigned int Animated::keyframe_get_total_num(void)
{
  return keyframes.size();
}


double Animated::step_factor_get(double time)
{
  double factor = 1.0;

  double max = keyframe_next_time_get() - keyframe_prev_time_get();
  double step = time - keyframe_prev_time_get();

  if (step != max) {
    factor = step / max;
  }

  return factor;
}


void Animated::keyframe_incement(int increment)
{
  keyframe_prev = keyframe_next;
  keyframe_next = keyframe_next + increment;
}


void Animated::step_time(double dt)
{
  if (dt <= 0)
    return;

  if (keyframes.size() == 0)
    return;

  animation_time = animation_time + dt;
  if (animation_time >= animation_time_max) {
    rewind();
  } else if (animation_time >= keyframe_next_time_get()) {
    keyframe_incement(1);
  }
  double factor = step_factor_get(animation_time);
  keyframe_interpolate(factor);

}

void Animated::rewind(void)
{
  keyframe_next = 1;
  keyframe_prev = 0;
  animation_time = 0;
}


/**************************************************/
/***************** PRIVATE METHODS ****************/
/**************************************************/
