#include "animated.h"

#include <memory>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "utils.h"


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


void Animated::keyframe_add(glm::vec3 s, glm::quat q, glm::vec3 t, double time)
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


void Animated::keyframe_print_all()
{
  for (auto &kf : keyframes) {
    std::cout << kf->time << std::endl;
  }

}

void Animated::keyframe_range_activate(const std::string &name)
{
  if (keyframe_total_num_get() <= 0) {
    std::cout << "Error: no keyframes exist" << std::endl;
    return;
  }
  current = &animations[name];

  int first = current->keyframe_first;
  int last = current->keyframe_last;

  std::cout << "Setting keyframe to active: " << current->name << std::endl;
  std::cout << "From: " << first << " To: " << last << std::endl;
  std::cout << "Time From: " << keyframes[first]->time << " To: " << keyframes[last]->time  << std::endl;

}


void Animated::keyframe_range_set(const std::string &name, const unsigned int keyframe_first, const unsigned int keyframe_last)
{
  Animation a;

  a.keyframe_first = keyframe_first;
  a.keyframe_last = keyframe_last;
  a.name = name;
  animations[name] = a;
}


unsigned int Animated::keyframe_total_num_get()
{
  return keyframes.size();
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
  transform_local_interpolated = m;

  return m;
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

  return animation_time_min;
}


double Animated::keyframe_next_time_get()
{
  assert(keyframes.size() > 0);

  if (keyframes[keyframe_next])
    return keyframes[keyframe_next]->time;

  return animation_time_max;
}


int Animated::keyframe_next_get()
{
  return keyframe_next;
}


int Animated::keyframe_prev_get()
{
  return keyframe_prev;
}


void Animated::rewind()
{
  keyframe_next = 1;
  keyframe_prev = 0;
  animation_time = 0;
}

