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
  : maxAnimTime(0),
  minAnimTime(0),
  nextKeyFrame(1),
  prevKeyFrame(0),
  animationTime(0)
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
  std::unique_ptr <KeyFrame> kf_ptr(new KeyFrame(s, q, t, time));
  KeyFrame & kf = *kf_ptr;
  keyFrames.push_back(std::move(kf_ptr));

  if (maxAnimTime < kf.time) {
    maxAnimTime = kf.time;
  }
  if (minAnimTime > kf.time) {
    minAnimTime = kf.time;
  }
}


void Animated::keyframe_delete_all()
{
  keyFrames.clear();
}


glm::mat4 Animated::keyframe_interpolate(double factor)
{
  vec3 tInterp, sInterp;
  quat qInterp;

  int prevFrame = keyframe_prev_get();
  int nextFrame = keyframe_next_get();

  /* Scaling */
  if (keyFrames[prevFrame]->s != keyFrames[nextFrame]->s) {
    sInterp = mix(keyFrames[prevFrame]->s,
        keyFrames[nextFrame]->s, (float) factor);
  } else {
    sInterp = keyFrames[prevFrame]->s;
  }

  /* Translation */
  if (keyFrames[prevFrame]->t != keyFrames[nextFrame]->t) {
    tInterp = mix(keyFrames[prevFrame]->t,
        keyFrames[nextFrame]->t, (float) factor);
  } else {
    tInterp = keyFrames[prevFrame]->t;
  }

  /* Rotation */
  if (keyFrames[prevFrame]->q != keyFrames[nextFrame]->q) {
    qInterp = mixQuat(keyFrames[prevFrame]->q,
        keyFrames[nextFrame]->q, (float) factor);
  } else {
    qInterp = keyFrames[prevFrame]->q;
  }

  mat4 m = glm::translate(glm::mat4(1), tInterp) *
    glm::mat4_cast(qInterp) * glm::scale(glm::mat4(1), sInterp);
  localInterpolatedMatrix = m;

  return m;
}


double Animated::keyframe_prev_time_get(void)
{
  if (nextKeyFrame != 0)
    return this->keyFrames[this->prevKeyFrame]->time;

  return this->minAnimTime;
}


double Animated::keyframe_next_time_get(void)
{
  assert(keyFrames.size() > 0);

  if (keyFrames[this->nextKeyFrame])
    return keyFrames[this->nextKeyFrame]->time;

  return this->maxAnimTime;
}


int Animated::keyframe_next_get(void)
{
  return this->nextKeyFrame;
}


int Animated::keyframe_prev_get(void)
{
  return this->prevKeyFrame;
}


unsigned int Animated::getNumKeyFrames(void)
{
  return keyFrames.size();
}

double Animated::getStepFactor(double time)
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
  prevKeyFrame = nextKeyFrame;
  nextKeyFrame = nextKeyFrame + increment;
}


bool Animated::stepAnimation(double time)
{
  if (time >= maxAnimTime) {
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

  if (keyFrames.size() == 0)
    return;

  animationTime = animationTime + dt;
  stepAnimation(animationTime);
  double factor = getStepFactor(animationTime);
  keyframe_interpolate(factor);

}

void Animated::rewind(void)
{
  nextKeyFrame = 1;
  prevKeyFrame = 0;
  animationTime = 0;
}
