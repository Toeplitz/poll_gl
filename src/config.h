#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <functional>


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


class Console;
class Scene;
class GLcontext;


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
    Console *console;
    Scene *scene;
    GLcontext *glcontext;

    Conf_Global conf_global;
    std::string conf_global_file;
    Json::Value conf_global_json;

    template <typename T>
    void  add(Json::Value &conf, const std::string &root, const std::string &prim, const std::string &sec, const T &value);
    void  callback_conf_global_value_set(const std::string &prim, const std::string &sec, const std::string &val);
    void  conf_global_parse();
    void  conf_global_console_add(const std::string &prim, const std::string &sec);
    template <typename T>
    void conf_global_init(T &local, const std::string &prim, const std::string &sec, bool console_add);
    template <typename T>
    void  conf_global_set(T &local, const std::string &prim, const std::string &sec);
    void  conf_global_set_all(bool console_add = false);
    void  write(Json::Value &conf);
    void  write_defaults();

  public:

    Config();
    ~Config();

    void                init(Console &console, Scene &scene, GLcontext &glcontext, const std::string &conf_global_file);
    void                conf_global_apply(const std::string &prim);
    const Conf_Global  &conf_global_get();

};
