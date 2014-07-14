#include "fragmic.h"
#include <iostream>
#include <memory>
#include <vector>
#include <functional>

using std::cout;
using std::endl;
using std::vector;

class Demo {
  public:

    Fragmic fragmic;

    Demo(const std::string &title, const int &width, const int &height) 
      : fragmic(title, width, height)
    {
      Window &window = fragmic.window_get();
      window.keyboard_pressed_callback_set(this, &Demo::keyboardPressedCallback);
    }

    ~Demo() 
    {

    }

    void run()
    {
      Transform t;
      Physics &physics = fragmic.physics_get();
      Scene &scene = fragmic.scene_get();

      Node &node = scene.model_load("data/game_assets/", "box.dae", MODEL_IMPORT_OPTIMIZED);
      physics.collision_node_add(node, PHYSICS_COLLISION_BOX, true, 1.f);
      physics.pause();
      scene.scene_graph_print(false);

      fragmic.run();
      fragmic.term();
    }

    void load()
    {
      Transform t;
      Scene &scene = fragmic.scene_get();

      Node &node = scene.model_load("data/zombie/", "new_thin_zombie.dae", MODEL_IMPORT_OPTIMIZED);
      t.translate(node, glm::vec3(-2.5, 2.5, 0));

      Node &node2 = scene.model_load("data/bob/", "Bob_with_lamp.dae", MODEL_IMPORT_OPTIMIZED);
      t.translate(node2, glm::vec3(2.5, 2.5, 0));
      scene.scene_graph_print(false);
    }

    void keyboardPressedCallback(SDL_Keysym *keysym)
    {
      Physics &physics = fragmic.physics_get();

      switch (keysym->sym) {
        case SDLK_l:
          load();
          break;
        case SDLK_o:
          physics.debug();
          break;
      }
    }

};


int main() 
{
  Demo demo("Demo 1", 1280, 720);
  demo.run();

  return true;
}
