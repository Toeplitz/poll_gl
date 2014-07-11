#include "animated.h"

#include <memory>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "armature.h"
#include "utils.h"



/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Animated::Animated():
  animation_time(0),
  global_time_max(0),
  global_time_min(0),
  keyframe_next(1),
  keyframe_prev(0),
  keyframe_range_next(1),
  keyframe_range_prev(0)
{
}


Animated::~Animated()
{
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Animated::animation_activate(const Animation &a)
{
  unsigned int first = a.keyframe_first;
  unsigned int last = a.keyframe_last;
  std::cout << "\tSetting keyframe limits for "<< std::endl;

  keyframe_range_prev = first;
  keyframe_range_next = keyframe_range_prev + 1;

  animation_time_min = keyframes[first]->time;
  animation_time_max = keyframes[last]->time;

  reset();

  std::cout << "time start/stop: " << keyframes[first]->time << " / " << keyframes[last]->time << std::endl;
}


void Animated::keyframe_add(glm::vec3 s, glm::quat q, glm::vec3 t, double time)
{
  std::unique_ptr <Keyframe> kf_ptr(new Keyframe(s, q, t, time));
  Keyframe &kf = *kf_ptr;
  keyframes.push_back(std::move(kf_ptr));

  if (global_time_max < kf.time) {
    global_time_max = kf.time;
    animation_time_max = global_time_max;
  }
  if (global_time_min > kf.time) {
    global_time_min = kf.time;
    animation_time_min = global_time_min;
  }
}


glm::mat4 Animated::keyframe_interpolate(double factor)
{
  glm::vec3 tInterp, sInterp;
  glm::quat qInterp;

  int frame_prev = keyframe_prev_get();
  int frame_next = keyframe_next_get();

  /* Scaling */
  if (keyframes[frame_prev]->s != keyframes[frame_next]->s) {
    sInterp = glm::mix(keyframes[frame_prev]->s,
        keyframes[frame_next]->s, (float) factor);
  } else {
    sInterp = keyframes[frame_prev]->s;
  }

  /* Translation */
  if (keyframes[frame_prev]->t != keyframes[frame_next]->t) {
    tInterp = glm::mix(keyframes[frame_prev]->t,
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

  glm::mat4 m = glm::translate(glm::mat4(1), tInterp) *
    glm::mat4_cast(qInterp) * glm::scale(glm::mat4(1), sInterp);

  return m;
}


void Animated::keyframe_print_all()
{
  for (auto &kf : keyframes) {
    std::cout << kf->time << std::endl;
  }

}


unsigned int Animated::keyframe_total_num_get()
{
  return keyframes.size();
}


float Animated::step_time(const double dt)
{
  if (dt <= 0)
    return -1;

  if (keyframes.size() == 0)
    return -1;

  animation_time = animation_time + dt;
  if (animation_time >= animation_time_max) {
    reset();
  } else if (animation_time >= keyframe_next_time_get()) {
    keyframe_incement(1);
  }
  double factor = step_factor_get(animation_time);

  return factor;
}


/**************************************************/
/***************** PRIVATE METHODS ****************/
/**************************************************/


void Animated::keyframe_delete_all()
{
  keyframes.clear();
}


void Animated::keyframe_incement(int increment)
{
  keyframe_prev = keyframe_next;
  keyframe_next = keyframe_next + increment;
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


double Animated::keyframe_prev_time_get()
{
  if (keyframe_next != 0)
    return keyframes[keyframe_prev]->time;

  return global_time_min;
}


double Animated::keyframe_next_time_get()
{
  assert(keyframes.size() > 0);

  if (keyframes[keyframe_next])
    return keyframes[keyframe_next]->time;

  return global_time_max;
}


int Animated::keyframe_next_get()
{
  return keyframe_next;
}


int Animated::keyframe_prev_get()
{
  return keyframe_prev;
}


void Animated::reset()
{
  keyframe_next = keyframe_range_next;
  keyframe_prev = keyframe_range_prev;
  animation_time = animation_time_min;
}

