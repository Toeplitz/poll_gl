#include "plugin_debug.h"



Plugin_Debug::Plugin_Debug(Console &console, Scene &scene)
{
  this->console = &console;
  this->scene = &scene;
  this->physics = &scene.physics_get();
}


void Plugin_Debug::cb_keyboard_pressed(SDL_Keysym *keysym)
{

  switch (keysym->sym) {
    case SDLK_RETURN:
      console->toggle();
      break;
    default:
      break;
  }

  if (console->active()) 
    return;

  switch (keysym->sym) {
    case SDLK_f:
      physics->pause();
      break;
    case SDLK_o:
      physics->debug();
      break;
    default:
      break;
  }

}


