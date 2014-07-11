#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "keyframe.h"

struct Animation;


struct Animated 
{
  private:
    double animation_time;
    double animation_time_min;
    double animation_time_max;
    double global_time_max;
    double global_time_min;
    std::vector<std::unique_ptr<Keyframe>> keyframes;
    unsigned int keyframe_next;
    unsigned int keyframe_prev;
    unsigned int keyframe_range_next;
    unsigned int keyframe_range_prev;

    void          keyframe_delete_all();
    void          keyframe_incement(int increment);
    int           keyframe_prev_get();
    double        keyframe_prev_time_get();
    int           keyframe_next_get();
    double        keyframe_next_time_get();
    void          reset();
    double        step_factor_get(double time);


  public:

    Animated();
    ~Animated();

    void          animation_activate(const Animation &a);
    void          keyframe_add(glm::vec3 s, glm::quat q, glm::vec3 t, double time);
    glm::mat4     keyframe_interpolate(double factor);
    void          keyframe_print_all();
    unsigned int  keyframe_total_num_get();
    float         step_time(const double dt);
};

