#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <jsoncpp/json/json.h>


#define CONF_GLOBAL                            "global"
#define CONF_GLOBAL_CAMERA                     "camera"
#define CONF_GLOBAL_CAMERA_FOV                 "fov"
#define CONF_GLOBAL_VIEWPORT                   "viewport"
#define CONF_GLOBAL_VIEWPORT_WIDTH             "width"
#define CONF_GLOBAL_VIEWPORT_HEIGHT            "height"
#define CONF_GLOBAL_SSAO                       "ssao"
#define CONF_GLOBAL_SSAO_SAMPLECOUNT           "sample_count"
#define CONF_GLOBAL_SSAO_DISTANCETHRESHOLD     "distance_threshold"
#define CONF_GLOBAL_SSAO_FILTERRADIUS          "filter_radius"


struct Conf_Global
{
  struct Camera {
    float fov;
  } camera;

  struct Viewport {
    int height;
    int width;
  } viewport;

  struct Ssao {
    int sample_count;
    float distance_threshold;
    float filter_radius;
  } ssao;

};


struct Conf_Scene
{
  struct Light {
    glm::vec3 position;

  } light;

};


class Config
{
  private:
    std::string global_file;
    Conf_Global conf_global;
    Conf_Scene conf_scene;

  public:

    Config();
    ~Config();


    void                init(const std::string &global_file);
    void                write_default();
    const Conf_Global  &parse_global();

};
