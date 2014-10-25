#pragma once

#include "poll.h"
#include "poll_plugin.h"
#include "raycast.h"

#include <map>
#include <memory>
#include <vector>

using glm::vec3;


class Plugin_Node_Tool: public Poll_Plugin
{
  private:
    Console *console = nullptr;
    Scene *scene = nullptr;

    Raycast raycast;
  
    std::map<SDL_Keycode, std::pair<bool, std::string>> keypress_map;

    bool mouse_down = false;
    std::shared_ptr<Raycast_Hitpoint> hitpoint_last;
    std::vector<std::unique_ptr<Physics_Triangle_Mesh_Shape>> shapes;

    Node *node_gizmo;
    Node *node_gizmo_translate_x;
    Node *node_gizmo_translate_y;
    Node *node_gizmo_translate_z;
    Node *node_bounding_box;

    std::vector<Raycast_Hitpoint> hitpoints;

    Raycast_Hitpoint *hitpoint_last_get();
    Node             *hitpoint_last_node_get();
    Node             *node_motion = nullptr;


  public:
    Plugin_Node_Tool(Console &console, Scene &scene);
    void cb_keyboard_pressed(SDL_Keysym *keysym);
    void cb_keyboard_released(SDL_Keysym *keysym);
    void cb_mouse_pressed(SDL_MouseButtonEvent *ev);
    void cb_mouse_released(SDL_MouseButtonEvent *ev);
    void cb_mouse_motion(SDL_MouseMotionEvent *ev);
    void cb_mouse_motion_old(SDL_MouseMotionEvent *ev);
    void cb_node_draw(Node &node);

};
