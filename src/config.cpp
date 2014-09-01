#include "config.h"
#include "console.h"
#include "glcontext.h"
#include "scene.h"
#include "utils.h"
#include <fstream>
#include <functional>
#include <streambuf>


using namespace std::placeholders;


template<class T>
T convert_type_json(Json::Value &val)
{
  return T();
}

template <>
float convert_type_json<float>(Json::Value &val)
{
  return val.asFloat();
}


template <>
int convert_type_json<int>(Json::Value &val)
{
  return val.asInt();
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Config::conf_global_apply(const std::string &prim)
{
  if (!prim.compare(CONF_GLOBAL) || !prim.compare(CONF_GLOBAL_CAMERA))
  {
    Camera *camera = scene->camera_get();
    camera->fov_set(conf_global.camera.fov);
  }

  if (!prim.compare(CONF_GLOBAL) || !prim.compare(CONF_GLOBAL_VIEWPORT))
  {
    Window &window = scene->window_get();
    window.swap_interval_set(conf_global.viewport.swap_interval);
  }


  if (prim.compare(CONF_GLOBAL) || prim.compare(CONF_GLOBAL_SSAO) 
      || prim.compare(CONF_GLOBAL_VIEWPORT))
  {
    GLcontext &glcontext = scene->glcontext_get();
    glcontext.uniform_buffers_update_config(*this);
  }
}


void Config::init(Console &console, Scene &scene, const std::string &global_file)
{
  this->console = &console;
  this->scene = &scene;
  this->global_file = global_file;

  if (!file_exists(global_file)) {
    std::cout << "No global config file, creating: '" << global_file << "'" << std::endl;
    write_defaults();
  }

  conf_global_parse();
}


const Conf_Global &Config::conf_global_get()
{
  return conf_global;
}


/**************************************************/
/***************** PRIVATE METHODS ****************/
/**************************************************/


template <typename T>
void Config::add(Json::Value &conf, const std::string &root, const std::string &prim, const std::string &sec, const T &value)
{
  conf[root][prim][sec] = value;
}


void Config::callback_conf_global_value_set(const std::string &prim, const std::string &sec, const std::string &val) 
{
  Json::Value &value = conf_global_json[CONF_GLOBAL][prim][sec];

  if (val.size() <= 0)
    return;

  if (value.isDouble()) {
    value = ::atof(val.c_str());
  } else if (value.isInt()) {
    value = ::atoi(val.c_str());
  }

  conf_global_set_all(false);
  conf_global_apply(prim);
}


void Config::conf_global_console_add(const std::string &prim, const std::string &sec)
{
  console->command_add(prim, sec, std::bind(&Config::callback_conf_global_value_set, this, _1, _2, _3));
}


template <typename T>
void Config::conf_global_set(T &local, const std::string &prim, const std::string &sec)
{
  local = convert_type_json<T>(conf_global_json[CONF_GLOBAL][prim][sec]);
}


void Config::conf_global_set_all(bool console_add)
{
  conf_global_init(conf_global.camera.fov, CONF_GLOBAL_CAMERA, CONF_GLOBAL_CAMERA_FOV, console_add);
  conf_global_init(conf_global.viewport.width, CONF_GLOBAL_VIEWPORT, CONF_GLOBAL_VIEWPORT_WIDTH, console_add);
  conf_global_init(conf_global.viewport.width, CONF_GLOBAL_VIEWPORT, CONF_GLOBAL_VIEWPORT_SWAPINTERVAL, console_add);
  conf_global_init(conf_global.viewport.height, CONF_GLOBAL_VIEWPORT, CONF_GLOBAL_VIEWPORT_HEIGHT, console_add);
  conf_global_init(conf_global.ssao.sample_count, CONF_GLOBAL_SSAO, CONF_GLOBAL_SSAO_SAMPLECOUNT, console_add);
  conf_global_init(conf_global.ssao.distance_threshold, CONF_GLOBAL_SSAO, CONF_GLOBAL_SSAO_DISTANCETHRESHOLD, console_add);
  conf_global_init(conf_global.ssao.filter_radius, CONF_GLOBAL_SSAO, CONF_GLOBAL_SSAO_FILTERRADIUS, console_add);
}


template <typename T>
void Config::conf_global_init(T &local, const std::string &prim, const std::string &sec, bool console_add)
{
  conf_global_set(local, prim, sec);

  if (console_add)
    conf_global_console_add(prim, sec);
}


void Config::conf_global_parse()
{
  if (global_file.empty()) {
    std::cout << "Error: config needs to be initialized with a config file" << std::endl;
    return;
  } 

  std::ifstream t(global_file);
  std::string str((std::istreambuf_iterator<char>(t)),
                   std::istreambuf_iterator<char>());

  Json::Reader reader;

  bool ret = reader.parse(str, conf_global_json);
  if (!ret) {
    std::cout << "Error: json parse error" << std::endl;
    return;
  }

  conf_global_set_all(true);
}


void Config::write(Json::Value &conf)
{
  if (global_file.empty()) {
    POLL_ERROR(std::cerr, "Config needs to be initialized with a config file!");
    return;
  }

  std::ofstream file;
  file.open(global_file);
  Json::StyledWriter writer;
  file << writer.write(conf);
  file.close();
}


void Config::write_defaults()
{
  Json::Value conf;

  add<int>(conf, CONF_GLOBAL, CONF_GLOBAL_VIEWPORT, CONF_GLOBAL_VIEWPORT_WIDTH, 1280);
  add<int>(conf, CONF_GLOBAL, CONF_GLOBAL_VIEWPORT, CONF_GLOBAL_VIEWPORT_HEIGHT, 720);
  add<int>(conf, CONF_GLOBAL, CONF_GLOBAL_VIEWPORT, CONF_GLOBAL_VIEWPORT_SWAPINTERVAL, 1);
  add<float>(conf, CONF_GLOBAL, CONF_GLOBAL_CAMERA, CONF_GLOBAL_CAMERA_FOV, 45.0);
  add<int>(conf, CONF_GLOBAL, CONF_GLOBAL_SSAO, CONF_GLOBAL_SSAO_SAMPLECOUNT, 16);
  add<float>(conf, CONF_GLOBAL, CONF_GLOBAL_SSAO, CONF_GLOBAL_SSAO_DISTANCETHRESHOLD, 5.0);
  add<float>(conf, CONF_GLOBAL, CONF_GLOBAL_SSAO, CONF_GLOBAL_SSAO_FILTERRADIUS, 10.0);

  write(conf);
}
