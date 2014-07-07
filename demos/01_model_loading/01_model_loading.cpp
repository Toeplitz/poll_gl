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

      scene.load_model("data/", "scene_textured.dae");

      scene.scene_graph_print();
      assets.print_all();

      fragmic.run();
      fragmic.term();
    }

    void load()
    {
      Transform t;
      Scene &scene = fragmic.scene_get();
      Assets &assets = scene.assets_get();

      Node &node = scene.load_model("data/zombie/", "new_thin_zombie.dae");
      t.translate(node, glm::vec3(-2.5, 2.5, 0));

      Node &node2 = scene.load_model("data/bob/", "Bob_with_lamp.dae");
      t.translate(node2, glm::vec3(2.5, 2.5, 0));
      scene.scene_graph_print();
      assets.print_all();
    }

    void keyboardPressedCallback(SDL_Keysym *keysym)
    {
      vector<glm::vec3> wayPoints;
      vector<Uint32> wayPointTime;
      float n = 90.0;
      float maxTime = 3000; 

      Camera &camera = fragmic.camera_get();
      Window &window = fragmic.window_get();

      switch (keysym->sym) {
        case SDLK_d:
          window.debug_toggle();
          break;
        case SDLK_l:
          load();
          break;
        case SDLK_c:
          wayPoints.reserve(n);
          wayPointTime.reserve(n);
          for (int i = 1; i <= (int) n; i++) {
            float theta = i * 2*M_PI / n;
            float phi = 0;
            float time = i * maxTime / n;
            float r = 10;
            float x = r * sin(theta) * cos(phi);
            float y = r * sin(theta) * sin(phi) + 5;
            float z = r * cos(theta);
            wayPoints.push_back(glm::vec3(x, y, z));
            wayPointTime.push_back(time);
          }

          camera.setWayPoints(wayPoints, wayPointTime, glm::vec3(0,3,0), true);

          break;
        default:
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
