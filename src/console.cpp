#include "console.h"
#include <algorithm>
#include <iterator>
#include <string>
#include <sstream>
#include <vector>
#include <glm/gtx/string_cast.hpp>


using namespace std::placeholders;


/**************************************************/
/***************** CONSTRUCTORS *******************/
/**************************************************/


Console::Console()
{
}


Console::~Console()
{
}


/**************************************************/
/***************** PUBLIC METHODS *****************/
/**************************************************/


bool Console::active()
{
  return flag_toggle;
}


void Console::command_add(const std::string &prim, const std::string &sec, 
    std::function<void (const std::string &, const std::string &, const std::string &)> cb)
{
  commands[std::make_pair(prim, sec)] = std::bind(cb, _1, _2, _3);
}


void Console::draw()
{
  if (!flag_toggle)
    return;

  glcontext->draw_text(*node_text);
}


Font *Console::font_get() 
{
  return &font;
}


void Console::init(GLcontext &glcontext, Scene &scene, Window &window)
{
  this->scene = &scene;
  this->glcontext = &glcontext;

  window.keyboard_pressed_callback_set(this, &Console::keyboard_pressed_cb);

  font_create(CONSOLE_FONT);

  node_text = scene.node_create("entry_box");
  Text *text = node_text->text_create(&font, scene);
  text->string_set("default");
  text->bake(nullptr, node_text->mesh_get(), CONSOLE_X, CONSOLE_Y);
  glcontext.vertex_buffers_mesh_create(node_text->mesh_get(), 1048 * sizeof(glm::vec3));
}


void Console::keyboard_pressed_cb(SDL_Keysym *keysym)
{
  Text *text;

  if (!flag_toggle)
    return;

  text = node_text->text_get();

  switch (keysym->sym) {
    case SDLK_RETURN:
      break;
    case SDLK_SPACE:
      text->string_append(" ");
      text->bake(glcontext, node_text->mesh_get(), CONSOLE_X, CONSOLE_Y);
      break;
    case SDLK_BACKSPACE:
      if (text->string_len() > 2) {
        text->string_erase_last();
        text->bake(glcontext, node_text->mesh_get(), CONSOLE_X, CONSOLE_Y);
      }
      break;
    case SDLK_TAB:
      command_tab_complete(text->string_get());
      break;
    case SDLK_UP:
      command_history_get_prev();
      break;
    case SDLK_DOWN:
      command_history_get_next();
      break;
    default:
      if((keysym->sym >= SDLK_a && keysym->sym <= SDLK_z) ||
          (keysym->sym >= SDLK_0 && keysym->sym <= SDLK_9) ||
          keysym->sym == SDLK_PERIOD) {
        std::string key_input(SDL_GetKeyName(keysym->sym));
        std::transform(key_input.begin(), key_input.end(), key_input.begin(), ::tolower);
        text->string_append(key_input);
        text->bake(glcontext, node_text->mesh_get(), CONSOLE_X, CONSOLE_Y);
      } 
      break;
  }

}


void Console::toggle()
{
  Text *text;
  text = node_text->text_get();

  if (flag_toggle) {
    std::string &cmd_full = text->string_get();
    std::string cmd = cmd_full.substr(2, cmd_full.size());
    command_parse(cmd);
  } else {
    text->string_set(CONSOLE_PREFIX);
    text->bake(glcontext, node_text->mesh_get(), CONSOLE_X, CONSOLE_Y);
  }

  flag_toggle = !flag_toggle;
}


void Console::term()
{
  font_delete();
}



/**************************************************/
/***************** PRIVATE METHODS ****************/
/**************************************************/


void Console::command_exec(const std::string &prim, const std::string &sec, const std::string &value)
{
  if (commands.find(std::make_pair(prim, sec)) != commands.end()) {
    commands.at(std::make_pair(prim, sec))(prim, sec, value);
  } else {
    std::cout << "Unknown command: '" << prim << " " << sec << "'" << std::endl;
  }
}


void Console::command_history_add(const std::string &cmd)
{
  history.push_back(cmd);
  history_location = history.size();
}


void Console::command_history_get_next()
{
  if (history.size() <= 0)
    return;

  command_history_show(history_location + 1);
}


void Console::command_history_get_prev()
{
  if (history.size() <= 0)
    return;

  command_history_show(history_location - 1);
}


void Console::command_history_show(const int loc)
{
  Text *text;

  history_location = loc;
  if (loc < 0) {
    history_location = 0;
    return;
  }
  if (static_cast<unsigned int>(loc) >= history.size()) {
    history_location = history.size() - 1;
    return;
  }

  text = node_text->text_get();
  text->string_set(CONSOLE_PREFIX + history[loc]);
  text->bake(glcontext, node_text->mesh_get(), CONSOLE_X, CONSOLE_Y);
}


std::string Console::command_tab_complete_find_prim_match(const std::string &str) 
{
  std::vector<std::string> unique;
  std::string ret("");

  for (Command_Map::iterator it = commands.begin(); it != commands.end(); ++it) {
    std::string key = it->first.first;
    if (std::find(unique.begin(), unique.end(), key) != unique.end()) {
      continue;
    }
    if (!key.compare(0, str.size(), str)) {
      ret = key;
    }
    unique.push_back(key);
  }

  return ret;
}


std::string Console::command_tab_complete_find_sec_match(const std::string &prim_key, const std::string &str) 
{
  std::string ret("");

  for (Command_Map::iterator it = commands.begin(); it != commands.end(); ++it) {
    std::string key = it->first.first;
    std::string sec_key = it->first.second;
    if (prim_key.compare(key))
      continue;
    if (!sec_key.compare(0, str.size(), str)) {
      ret = sec_key;
    }
  }

  return ret;
}

void Console::command_tab_complete(const std::string &cmd_full)
{
  Text *text;
  std::string match;

  std::istringstream iss(cmd_full);
  std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},
    std::istream_iterator<std::string>{}};

  text = node_text->text_get();
  switch (tokens.size()) {
    case 2:
      match = command_tab_complete_find_prim_match(tokens[1]);
      text->string_set(CONSOLE_PREFIX + match + " ");
      text->bake(glcontext, node_text->mesh_get(), CONSOLE_X, CONSOLE_Y);
      break;
    case 3:
      match = command_tab_complete_find_sec_match(tokens[1], tokens[2]);
      text->string_set(CONSOLE_PREFIX + tokens[1] + " " + match + " ");
      text->bake(glcontext, node_text->mesh_get(), CONSOLE_X, CONSOLE_Y);
    default:
      break;
  }

}


void Console::command_parse(std::string &cmd_full)
{
  std::istringstream iss(cmd_full);
  std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},
    std::istream_iterator<std::string>{}};

  if (tokens.size() == 2) {
    command_exec(tokens[0], tokens[1], "");
  } else if (tokens.size() == 3) {
    command_exec(tokens[0], tokens[1], tokens[2]);
  }

  command_history_add(cmd_full);
}


void Console::font_create(const std::string &font_file)
{
  font.load(font_file);
  font.bake();
  Texture &texture = font.texture_get();
  glcontext->texture_single_channel_create(texture);
}



void Console::font_delete()
{
  glcontext->texture_delete(font.texture_get());
}


