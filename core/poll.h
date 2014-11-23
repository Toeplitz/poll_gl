#pragma once

#include <string>
#include "config.h"
#include "glcontext.h"                  
#include "glshader.h"                   
#include "physics.h"
#include "poll_plugin.h"
#include <vector>


#define GLOBAL_TITLE "Poll"
#define GLOBAL_CONFIG "./poll.conf"



class Poll {
  private:

//    Config config;
 //   Console console;
    GLcontext glcontext;
 //   GLui ui;


    std::string fps_text;
    std::string &fps_text_get();

  public:
    Poll_Plugin_List plugins;


    Poll(const std::string &config_file = ""); 

    Config      &config_get();
 //   Console     &console_get();
    GLcontext   &glcontext_get();
    void         init();
    double       delta_time_get();
    void         profile_fps(const double dt);
    void         plugin_add(Poll_Plugin &plugin);
    void         step(Scene &scene);
    void         term();
};


