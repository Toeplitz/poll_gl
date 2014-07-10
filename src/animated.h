#pragma once

#include <vector>
#include <map>
#include <memory>
#include <glm/glm.hpp>

#include "keyframe.h"


struct Animation
{
  std::string name;
  unsigned int keyframe_first;
  unsigned int keyframe_last;
};


struct Animated 
{
  private:
    std::map<std::string, Animation> animations;
    Animation *current;

    double animation_time_max;
    double animation_time_min;
    int keyframe_next;
    int keyframe_prev;

  public:
    double animation_time;
    glm::mat4 transform_local_interpolated;
    std::vector<std::unique_ptr<Keyframe>> keyframes;

    Animated();
    ~Animated();

    void          add(const std::string &name, const unsigned int keyframe_first, const unsigned int keyframe_last);
    void          play(const std::string &name);


    void          keyframe_add(glm::vec3 s, glm::quat q, glm::vec3 t, double time);
    void          keyframe_delete_all();
    unsigned int  keyframe_get_total_num();
    void          keyframe_incement(int increment);
    glm::mat4     keyframe_interpolate(double factor);
    int           keyframe_prev_get();
    double        keyframe_prev_time_get();
    int           keyframe_next_get();
    double        keyframe_next_time_get();
    void          rewind();
    bool          step(double time);
    double        step_factor_get(double time);
    void          step_time(double dt);
};

