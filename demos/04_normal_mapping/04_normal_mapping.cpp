#include "fragmic.h"
#include <iostream>

Fragmic fragmic("Demo 4", 1280, 720);

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

  scene.load_model("data/", "normal_map.dae", 0);

  scene.scene_graph_print();

  fragmic.run();
  fragmic.term();

  return true;
}
