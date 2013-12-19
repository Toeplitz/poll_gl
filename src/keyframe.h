#ifndef KEYFRAME_H_
#define KEYFRAME_H_

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

struct KeyFrame {
  double time;
   glm::vec3 s;                 /* scale */
   glm::quat q;                 /* rotate */
   glm::vec3 t;                 /* translate */

   KeyFrame(glm::vec3 _s, glm::quat _q, glm::vec3 _t, double _time)
  :time(_time), s(_s), q(_q), t(_t) {
  } ~KeyFrame() {

  }
};

#endif                          /* KEYFRAME_H_ */
