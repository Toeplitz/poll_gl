#include "config.h"
#include "utils.h"
#include <fstream>
#include <streambuf>


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Config::Config()
{
}


Config::~Config()
{
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


void Config::init(const std::string &global_file)
{
  this->global_file = global_file;

  if (!file_exists(global_file)) {
    std::cout << "No global config file, creating: '" << global_file << "'" << std::endl;
    write_default();
  }

  parse_global();
}


void Config::write_default()
{
  Json::Value conf;

  if (global_file.empty()) {
    std::cout << "Error: config needs to be initialized with a config file" << std::endl;
    return;
  }

  conf[CONF_GLOBAL][CONF_GLOBAL_VIEWPORT][CONF_GLOBAL_VIEWPORT_WIDTH]     = 1280;
  conf[CONF_GLOBAL][CONF_GLOBAL_VIEWPORT][CONF_GLOBAL_VIEWPORT_HEIGHT]    = 720;
  conf[CONF_GLOBAL][CONF_GLOBAL_CAMERA][CONF_GLOBAL_CAMERA_FOV]           = 45.0;
  conf[CONF_GLOBAL][CONF_GLOBAL_SSAO][CONF_GLOBAL_SSAO_SAMPLECOUNT]       = 16;
  conf[CONF_GLOBAL][CONF_GLOBAL_SSAO][CONF_GLOBAL_SSAO_DISTANCETHRESHOLD] = 5;
  conf[CONF_GLOBAL][CONF_GLOBAL_SSAO][CONF_GLOBAL_SSAO_FILTERRADIUS]      = 10;

  std::ofstream file;
  file.open(global_file);
  Json::StyledWriter writer;
  file << writer.write(conf);
  file.close();
}


const Conf_Global &Config::parse_global()
{
  if (global_file.empty()) {
    std::cout << "Error: config needs to be initialized with a config file" << std::endl;
    return conf_global;
  } 

  std::ifstream t(global_file);
  std::string str((std::istreambuf_iterator<char>(t)),
                   std::istreambuf_iterator<char>());

  Json::Value parsed;
  Json::Reader reader;

  bool ret = reader.parse(str, parsed);
  if (ret) {
    Json::StyledWriter writer;
    //std::cout << writer.write(parsed) << std::endl;
    conf_global.camera.fov              = parsed[CONF_GLOBAL][CONF_GLOBAL_CAMERA][CONF_GLOBAL_CAMERA_FOV].asFloat();
    conf_global.viewport.width          = parsed[CONF_GLOBAL][CONF_GLOBAL_VIEWPORT][CONF_GLOBAL_VIEWPORT_WIDTH].asInt();
    conf_global.viewport.height         = parsed[CONF_GLOBAL][CONF_GLOBAL_VIEWPORT][CONF_GLOBAL_VIEWPORT_HEIGHT].asInt();
    conf_global.ssao.sample_count       = parsed[CONF_GLOBAL][CONF_GLOBAL_SSAO][CONF_GLOBAL_SSAO_SAMPLECOUNT].asInt();
    conf_global.ssao.distance_threshold = parsed[CONF_GLOBAL][CONF_GLOBAL_SSAO][CONF_GLOBAL_SSAO_DISTANCETHRESHOLD].asFloat();
    conf_global.ssao.filter_radius      = parsed[CONF_GLOBAL][CONF_GLOBAL_SSAO][CONF_GLOBAL_SSAO_FILTERRADIUS].asFloat();
  }

  return conf_global;
}


