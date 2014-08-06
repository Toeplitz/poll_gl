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
    Font font;
    Node *node_console;
    Node *node_text;

    std::map<std::string, std::function<void ()>> commands;

    void command_add(const std::string &key, std::function<void (void *)>);
    void command_defaults_set();
    void command_exec(const std::string &key, const std::string &value);
    void command_parse(std::string &command_full);

  public:

    Console();
    ~Console();

    void init(Scene &scene, GLcontext &glcontext, Window &window);
    void draw();
    void keyboard_pressed_cb(SDL_Keysym *keysym);
    void toggle();
};
