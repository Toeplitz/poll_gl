#ifndef ANIMATED_H
#define ANIMATED_H

#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "keyframe.h"

struct Animated {
  glm::mat4 localInterpolatedMatrix;
  double maxAnimTime;
  double minAnimTime;
  int nextKeyFrame;
  int prevKeyFrame;
  std::vector < std::unique_ptr < KeyFrame >> keyFrames;
  double animationTime;

  Animated();

  void addKeyFrame(glm::vec3 s, glm::quat q, glm::vec3 t, double time);
  void deleteKeyFrames();
  int getPrevKeyFrame(void);
  int getNextKeyFrame(void);
  glm::mat4 interpolateTransforms(double factor);
  void rewindAnimation(void);
  bool stepAnimation(double time);
  void incrementKeyFrame(int incr);
  double getStepFactor(double time);
  unsigned int getNumKeyFrames(void);
  double getNextKeyFrameTime(void);
  double getPrevKeyFrameTime(void);
  void stepTime(double dt);
};

#endif
