#pragma once

#include <poll.h>
#include "poll_plugin.h"


class Plugin_Debug: public Poll_Plugin
{

  Console *console = nullptr;
  Scene *scene = nullptr;
  Physics *physics = nullptr;

  public:
    Plugin_Debug(Console &console, Scene &scene);
 //   void cb_keyboard_pressed(SDL_Keysym *keysym);

};
