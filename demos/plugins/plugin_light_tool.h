#pragma once
#include "poll.h"
#include "poll_plugin.h"

#include <memory>

using glm::vec3;


class Plugin_Light_Tool: public Poll_Plugin
{
  private:
    Console *console = nullptr;
    Scene *scene = nullptr;
    Light *light_active = nullptr;
    Node *node_symbol_cone = nullptr;
    std::unique_ptr<Physics_Convex_Hull_Shape> symbol_shape;

    void cb_custom_draw();
    void cb_light_create(Node *node);
    void console_cmd_light_create(const std::string &prim, const std::string &sec, const std::string &val);
    void console_cmd_light_disable(const std::string &prim, const std::string &sec, const std::string &val);
    void console_cmd_light_enable(const std::string &prim, const std::string &sec, const std::string &val);
    void console_cmd_light_list(const std::string &prim, const std::string &sec, const std::string &val);
    void console_cmd_light_select(const std::string &prim, const std::string &sec, const std::string &val);
    void console_cmd_light_type(const std::string &prim, const std::string &sec, const std::string &val);
    void console_cmd_light_print(const std::string &prim, const std::string &sec, const std::string &val);
    void light_active_set(Light *light);
    /*
    void raycast_collide_callback_gimbal_center(Node &node, vec3 &position);
    void raycast_collide_callback_gimbal_x(Node &node, vec3 &position);
    void raycast_collide_callback_gimbal_y(Node &node, vec3 &position);
    void raycast_collide_callback_gimbal_z(Node &node, vec3 &position);
    */



  public:
    Plugin_Light_Tool(Console &console, Scene &scene);


};
