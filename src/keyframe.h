#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Keyframe {
  double time;
  glm::vec3 s;                 /* scale */
  glm::quat q;                 /* rotate */
  glm::vec3 t;                 /* translate */

  Keyframe(glm::vec3 _s, glm::quat _q, glm::vec3 _t, double _time);
  ~Keyframe();
};

