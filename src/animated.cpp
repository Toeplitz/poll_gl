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

Animated::Animated()
:
maxAnimTime(0),
minAnimTime(0), nextKeyFrame(1), prevKeyFrame(0), animationTime(0)
{
}

void Animated::addKeyFrame(vec3 s, quat q, vec3 t, double time)
{
  std::unique_ptr < KeyFrame > kfPtr(new KeyFrame(s, q, t, time));
  KeyFrame & kf = *kfPtr;
  this->keyFrames.push_back(std::move(kfPtr));

  if (maxAnimTime < kf.time) {
    maxAnimTime = kf.time;
  }

  if (minAnimTime > kf.time) {
    minAnimTime = kf.time;
  }
}

void Animated::deleteKeyFrames()
{
  keyFrames.clear();
}

mat4 Animated::interpolateTransforms(double factor)
{
  vec3 tInterp, sInterp;
  quat qInterp;

  int prevFrame = this->getPrevKeyFrame();
  int nextFrame = this->getNextKeyFrame();

  /* Scaling */
  if (this->keyFrames[prevFrame]->s != this->keyFrames[nextFrame]->s) {
    sInterp = mix(this->keyFrames[prevFrame]->s,
                  this->keyFrames[nextFrame]->s, (float) factor);
  } else {
    sInterp = this->keyFrames[prevFrame]->s;
  }

  /* Translation */
  if (this->keyFrames[prevFrame]->t != this->keyFrames[nextFrame]->t) {
    tInterp = mix(this->keyFrames[prevFrame]->t,
                  this->keyFrames[nextFrame]->t, (float) factor);
  } else {
    tInterp = this->keyFrames[prevFrame]->t;
  }

  /* Rotation */
  if (this->keyFrames[prevFrame]->q != this->keyFrames[nextFrame]->q) {
    qInterp = mixQuat(this->keyFrames[prevFrame]->q,
                      this->keyFrames[nextFrame]->q, (float) factor);
  } else {
    qInterp = this->keyFrames[prevFrame]->q;
  }

  mat4 M = glm::translate(glm::mat4(1), tInterp) *
    glm::mat4_cast(qInterp) * glm::scale(glm::mat4(1), sInterp);

  localInterpolatedMatrix = M;

  return M;
}

double Animated::getPrevKeyFrameTime(void)
{
  if (nextKeyFrame != 0)
    return this->keyFrames[this->prevKeyFrame]->time;

  return this->minAnimTime;
}

double Animated::getNextKeyFrameTime(void)
{
  assert(keyFrames.size() > 0);

  if (keyFrames[this->nextKeyFrame])
    return keyFrames[this->nextKeyFrame]->time;

  return this->maxAnimTime;
}

int Animated::getNextKeyFrame(void)
{
  return this->nextKeyFrame;
}

int Animated::getPrevKeyFrame(void)
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

  double max = this->getNextKeyFrameTime() - this->getPrevKeyFrameTime();
  double step = time - this->getPrevKeyFrameTime();

  if (step != max) {
    factor = step / max;
  }

  return factor;
}

void Animated::incrementKeyFrame(int incr)
{
  this->prevKeyFrame = this->nextKeyFrame;
  this->nextKeyFrame = this->nextKeyFrame + incr;
}


bool Animated::stepAnimation(double time)
{
  if (time >= maxAnimTime) {
    this->rewindAnimation();
    return false;
  }

  if (time >= this->getNextKeyFrameTime()) {
    this->incrementKeyFrame(1);
  }

  return true;
}

void Animated::stepTime(double dt)
{
  if (dt <= 0)
    return;

  if (keyFrames.size() == 0)
    return;

  animationTime = animationTime + dt;
  stepAnimation(animationTime);
  double factor = getStepFactor(animationTime);
  interpolateTransforms(factor);

}

void Animated::rewindAnimation(void)
{
  nextKeyFrame = 1;
  prevKeyFrame = 0;
  animationTime = 0;
}
