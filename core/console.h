#pragma once

#include <map>
#include <functional>
//#include "scene.h"
#include "text.h"


#define CONSOLE_PREFIX "> "
#define CONSOLE_FONT   "data/fonts/Vera.ttf"
#define CONSOLE_X 10
#define CONSOLE_Y 10


typedef std::map<std::pair<std::string, std::string>, 
        std::function<void (const std::string &, const std::string &, const std::string &)>> Command_Map;

class Console
{
#if 0
  private:
    bool flag_toggle = false;
    Scene *scene;
    Font font;
    Node *node_text;
    Command_Map commands;
    std::vector<std::string> history;
    unsigned int history_location = 0;

    void        command_exec(const std::string &prim, const std::string &sec, const std::string &value);
    void        command_history_add(const std::string &cmd);
    void        command_history_get_prev();
    void        command_history_get_next();
    void        command_history_show(const int loc);
    void        command_tab_complete(const std::string &cmd_full);
    std::string command_tab_complete_find_prim_match(const std::string &str);
    std::string command_tab_complete_find_sec_match(const std::string &prim_key, const std::string &str);
    void        command_parse(std::string &cmd_full);
    void        font_create(const std::string &font_file);
    void        font_delete();

  public:
    Console();
    ~Console();

    bool  active();
    void  command_add(const std::string &prim, const std::string &sec, 
                     std::function<void (const std::string &, const std::string &, const std::string &)> cb);
    void  draw();
    Font *font_get();
    void  init(Scene &scene);
    //void  keyboard_pressed_cb(SDL_Keysym *keysym);
    void  toggle();
    void  term();
#endif
};
