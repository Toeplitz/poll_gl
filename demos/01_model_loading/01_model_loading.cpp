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
      Scene &scene = fragmic.scene_get();
      Assets &assets = scene.assets_get();
      
      Camera &camera = fragmic.camera_get();
      camera.translate(glm::vec3(0, 3, 0));

      scene.model_load("data/", "scene_textured.dae");

      scene.scene_graph_print();

      fragmic.run();
      fragmic.term();
    }

    void load()
    {
      Transform t;
      Scene &scene = fragmic.scene_get();
      Assets &assets = scene.assets_get();

      Node &node = scene.model_load("data/zombie/", "new_thin_zombie.dae");
      t.translate(node, glm::vec3(-2.5, 2.5, 0));

      Node &node2 = scene.model_load("data/bob/", "Bob_with_lamp.dae");
      t.translate(node2, glm::vec3(2.5, 2.5, 0));
      scene.scene_graph_print();
    }

    void keyboardPressedCallback(SDL_Keysym *keysym)
    {

    }



};


int main() 
{
  Demo demo("Demo 1", 1280, 720);
  demo.run();

  return true;
}
