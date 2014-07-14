#include "fragmic.h"
#include <iostream>

Fragmic fragmic("Demo 2", 1280, 720);


void keyboard_pressed_cb(SDL_Keysym *keysym)
{

  switch (keysym->sym) {
    default:
      break;
  }

}

int main() 
{
  Scene &scene = fragmic.scene_get();
  Window &window = fragmic.window_get();
  window.keyboard_pressed_callback_set(keyboard_pressed_cb);

  scene.model_load("data/crytek-sponza/", "sponza.obj", MODEL_IMPORT_OPTIMIZED);
 // scene.model_load("data/crytek-sponza/", "banner.obj", MODEL_IMPORT_OPTIMIZED);

  scene.scene_graph_print(false);
  scene.assets_get().print_all(scene.node_root_get());


  fragmic.run();
  fragmic.term();

  return true;
}
