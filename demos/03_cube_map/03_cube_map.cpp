#include "fragmic.h"
#include <iostream>

Fragmic fragmic("Demo 2", 1280, 720);


int main() 
{
  Scene &scene = fragmic.scene_get();

  {
    Node &node = scene.model_load("data/game_assets/", "box.dae");
  }

  /*
  {
    Node &node = *scene.node_create("box");
    node.mesh->cube_generate(3.0f);
    node.material->color_set(glm::vec3(1.f, 0.f, 0.f), 
        glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 1.f), 40.f);
    scene.upload_queue_add(node);
  }
  */

  {
    Node &node = *scene.node_create("skybox");
    node.mesh->cube_generate(150.0f);
    Material &material = *node.material;
    material.cubemap_create("data/game_assets/skybox/SkyboxSet1/DarkStormy/", "DarkStormyFront2048.png",
        "DarkStormyBack2048.png", "bar.png", "foo.png", "DarkStormyLeft2048.png", "DarkStormyRight2048.png");
    scene.upload_queue_add(node);
  }

  scene.scene_graph_print();

  fragmic.run();
  fragmic.term();

  return true;
}
