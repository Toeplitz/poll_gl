#pragma once
#include "poll.h"
#include "poll_plugin.h"


class Plugin_Light_Tool: public Poll_Plugin
{
  private:
    Console *console = nullptr;
    Scene *scene = nullptr;
    Light *light_active = nullptr;

    void callback_light_create(const std::string &prim, const std::string &sec, const std::string &val);
    void callback_light_disable(const std::string &prim, const std::string &sec, const std::string &val);
    void callback_light_enable(const std::string &prim, const std::string &sec, const std::string &val);
    void callback_light_list(const std::string &prim, const std::string &sec, const std::string &val);
    void callback_light_select(const std::string &prim, const std::string &sec, const std::string &val);
    void callback_light_type(const std::string &prim, const std::string &sec, const std::string &val);
    void callback_light_print(const std::string &prim, const std::string &sec, const std::string &val);
    void custom_draw_callback();
    void light_active_set(Light *light);



  public:
    Plugin_Light_Tool(Console &console, Scene &scene);


};
