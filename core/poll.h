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

    Poll_Plugin_List plugins;

    std::string fps_text;
    std::string &fps_text_get();
    void         profile_fps(const double dt);

  public:


    Poll(const std::string &config_file = ""); 

    Config                 &config_get();
 //   Console              &console_get();
    GLcontext              &glcontext_get();
    void                    init();
    double                  delta_time_get();
    void                    plugin_add(Poll_Plugin &plugin);
    const Poll_Plugin_List &plugins_get() const;
    void                    step(Scene &scene);
    void                    term();
};


