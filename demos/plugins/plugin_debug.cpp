#include "plugin_debug.h"



Plugin_Debug::Plugin_Debug(Console &console, Scene &scene)
{
  this->console = &console;
  this->scene = &scene;
  this->physics = &scene.physics_get();
}


void Plugin_Debug::mouse_callback_pressed(SDL_MouseButtonEvent *ev)
{
  std::cout << "mouse pressed down" << std::endl;
}


void Plugin_Debug::keyboard_callback_pressed(SDL_Keysym *keysym)
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


