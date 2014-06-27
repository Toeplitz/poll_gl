#include "fragmic.h"
#include <iostream>

class Demo {
  public:

    Fragmic fragmic;

    Demo(const std::string &title, const int &width, const int &height) 
      : fragmic(title, width, height)
    {
    }

    ~Demo() 
    {

    }

    void run()
    {
      Scene &scene = fragmic.scene_get();
      scene.load_model("data/", "scene_textured.dae");

      fragmic.run();
      fragmic.term();
    }

};


int main() 
{
  Demo demo("Demo 2", 1280, 720);
  demo.run();

  return true;
}
