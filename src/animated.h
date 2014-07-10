#pragma once

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
  std::vector<std::unique_ptr<KeyFrame>> keyFrames;
  double animationTime;

  Animated();
  ~Animated();

  void       keyframe_add(glm::vec3 s, glm::quat q, glm::vec3 t, double time);
  void       keyframe_delete_all();
  void       keyframe_incement(int increment);
  glm::mat4  keyframe_interpolate(double factor);
  int        keyframe_prev_get();
  double     keyframe_prev_time_get();
  int        keyframe_next_get();
  double     keyframe_next_time_get();
  void       rewind();
  void       step_time(double dt);

  bool stepAnimation(double time);
  double getStepFactor(double time);
  unsigned int getNumKeyFrames(void);
};

