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


Animated::Animated()
  : animation_time_max(0),
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

  int prevFrame = keyframe_prev_get();
  int nextFrame = keyframe_next_get();

  /* Scaling */
  if (keyframes[prevFrame]->s != keyframes[nextFrame]->s) {
    sInterp = mix(keyframes[prevFrame]->s,
        keyframes[nextFrame]->s, (float) factor);
  } else {
    sInterp = keyframes[prevFrame]->s;
  }

  /* Translation */
  if (keyframes[prevFrame]->t != keyframes[nextFrame]->t) {
    tInterp = mix(keyframes[prevFrame]->t,
        keyframes[nextFrame]->t, (float) factor);
  } else {
    tInterp = keyframes[prevFrame]->t;
  }

  /* Rotation */
  if (keyframes[prevFrame]->q != keyframes[nextFrame]->q) {
    qInterp = mixQuat(keyframes[prevFrame]->q,
        keyframes[nextFrame]->q, (float) factor);
  } else {
    qInterp = keyframes[prevFrame]->q;
  }

  mat4 m = glm::translate(glm::mat4(1), tInterp) *
    glm::mat4_cast(qInterp) * glm::scale(glm::mat4(1), sInterp);
  transform_local_interpolated = m;

  return m;
}


double Animated::keyframe_prev_time_get(void)
{
  if (keyframe_next != 0)
    return this->keyframes[this->keyframe_prev]->time;

  return this->animation_time_min;
}


double Animated::keyframe_next_time_get(void)
{
  assert(keyframes.size() > 0);

  if (keyframes[this->keyframe_next])
    return keyframes[this->keyframe_next]->time;

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

  double max = keyframe_next_time_get() - this->keyframe_prev_time_get();
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


bool Animated::step(double time)
{
  if (time >= animation_time_max) {
    rewind();
    return false;
  }

  if (time >= keyframe_next_time_get()) {
    this->keyframe_incement(1);
  }

  return true;
}


void Animated::step_time(double dt)
{
  if (dt <= 0)
    return;

  if (keyframes.size() == 0)
    return;

  animation_time = animation_time + dt;
  step(animation_time);
  double factor = step_factor_get(animation_time);
  keyframe_interpolate(factor);

}

void Animated::rewind(void)
{
  keyframe_next = 1;
  keyframe_prev = 0;
  animation_time = 0;
}
