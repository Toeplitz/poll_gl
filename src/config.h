#pragma once

#include <iostream>
#include <jsoncpp/json/json.h>


struct Config_Global
{
  struct Camera {
    float fov;
  };

  struct Viewport {
    int height;
    int width;
  };

  struct SSAO {

  };


};


struct Config_Light 
{

};


class Config
{
  private:
    std::string global_file;
    Config_Global config_global;

  public:

    Config();
    ~Config();


    void init(const std::string &global_file);
    void output(const Json::Value & value);
    void write_default();
    void parse_global();

};
