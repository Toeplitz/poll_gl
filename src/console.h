#pragma once

#include <map>
#include <functional>
#include "glshader.h"
#include "glcontext.h"
#include "scene.h"
#include "text.h"
#include "window.h"


class Console
{
  private:

    bool flag_toggle;
    GLshader glshader_console;
    GLcontext *glcontext;
    Scene *scene;
    Font font;
    Node *node_console;
    Node *node_text;

    std::map<std::string, std::function<void (const float)>> commands;

    void callback_camera_fov_set(const float val);
    void callback_light_create(const float val);
    void command_add(const std::string &key, std::function<void (const float)> cb);
    void command_defaults_set();
    void command_exec(const std::string &key, const std::string &value);
    void command_parse(std::string &cmd_full);

    void foo();

  public:

    Console();
    ~Console();

    bool active();
    void init(Scene &scene, GLcontext &glcontext, Window &window);
    void draw();
    void keyboard_pressed_cb(SDL_Keysym *keysym);
    void toggle();
};
